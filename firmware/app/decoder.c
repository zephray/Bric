//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "hal_filesystem.h"
#include "hal_audio.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "waveheader.h"
#include "decoder.h"

static int16_t pcm_buffer[MAX_FRAME_SIZE];
// 0: waiting for DMA CB, 1: waiting for decoder
// DMA CB can set this from 0 to 1, decoder can reset this from 1 to 0.
static SemaphoreHandle_t pcm_buffer_sem = NULL;
static volatile size_t pcm_buffer_ready;
static volatile size_t pcm_buffer_size;

void dec_fetch_thread(void *arg) {
    DecoderContext *ctx = (DecoderContext *)arg;
    uint8_t *buf;

    buf = pvPortMalloc(QUEUE_OBJECT);
	if (!buf) {
		printf("Unable to allocate memory for fetcher buffer!\n");
	}

    while (ctx->offset < ctx->file_size) {
        int bytes;
        bytes = hal_fs_read(ctx->file, buf, QUEUE_OBJECT); // Could be -1
        if (bytes < 0)
            break;
        if (bytes < QUEUE_OBJECT)
            memset(buf + bytes, 0, QUEUE_OBJECT - bytes);
        xQueueSendToBack(ctx->input_queue, buf, portMAX_DELAY);
        ctx->offset = hal_fs_tell(ctx->file);
    }

	vPortFree(buf);
	vTaskSuspend(NULL);
}

// Max number of samples to be submitted once
#define MAX_WAV_SIZE 512

int dec_decode_wav(DecoderContext *ctx, const uint8_t *buf, int buf_level,
        void *pcm, mp3dec_frame_info_t *info) {
    uint32_t num_samples;

    if (ctx->bits_per_sample == 16) {
        uint16_t *outbuf = (uint16_t *)pcm;
        num_samples = buf_level / 2 / ctx->channels;
        if (num_samples > MAX_WAV_SIZE) num_samples = MAX_WAV_SIZE;
        for (uint32_t i = 0; i < num_samples * ctx->channels; i++) {
            *outbuf++ = *(uint16_t *)buf;
            buf += 2;
        }
        info->frame_bytes = num_samples * 2 * ctx->channels;
    }
    else if (ctx->bits_per_sample == 24) {
        uint32_t *outbuf = (uint32_t *)pcm;
        num_samples = buf_level / 3 / ctx->channels;
        if (num_samples > MAX_WAV_SIZE) num_samples = MAX_WAV_SIZE;
        for (uint32_t i = 0; i < num_samples * ctx->channels; i++) {
            uint32_t sample;
            sample = (buf[i] << 8 | buf[i + 1] << 16 | buf[i + 2] << 24);
            buf += 3;
            *outbuf++ = sample;
        }
        info->frame_bytes = num_samples * 3 * ctx->channels;
    }
    else if (ctx->bits_per_sample == 32) {
        uint32_t *outbuf = (uint32_t *)pcm;
        num_samples = buf_level / 4 / ctx->channels;
        if (num_samples > MAX_WAV_SIZE) num_samples = MAX_WAV_SIZE;
        for (uint32_t i = 0; i < num_samples * ctx->channels; i++) {
            *outbuf++ = *(uint32_t *)buf;
            buf += 4;
        }
        info->frame_bytes = num_samples * 4 * ctx->channels;
    }
    info->channels = ctx->channels;

    return num_samples;
}

void dec_decode_thread(void *arg) {
    DecoderContext *ctx = (DecoderContext *)arg;
    uint8_t *buf = NULL;
    int buf_level = 0;
    int samples;
    mp3dec_frame_info_t info;

    buf = pvPortMalloc(QUEUE_OBJECT * 2);
    if (!buf) {
    	printf("Unable to allocate memory for decoder buffer!\n");
    }

    while (ctx->processed < ctx->file_size) {
        // Try to decode samples
        if ((buf_level < QUEUE_OBJECT) // buffer it not filled up
        		// and there is more than one queue object that is not fetched
        		&& (ctx->file_size - ctx->processed - buf_level > QUEUE_OBJECT)) {
            xQueueReceive(ctx->input_queue, buf + buf_level, portMAX_DELAY);
            buf_level += QUEUE_OBJECT;
        }
        // Wait for the output buffer to be available
        if (pcm_buffer_ready == 1)
        	xSemaphoreTake(pcm_buffer_sem, portMAX_DELAY);
        // Decode audio
        // This things need ~16KB stack space
        if (ctx->fmt == FMT_MP3) {
            samples = mp3dec_decode_frame(&(ctx->mp3d), buf, buf_level, pcm_buffer, &info);
        }
        else {
            samples = dec_decode_wav(ctx, buf, buf_level, pcm_buffer, &info);
        }
        buf_level -= info.frame_bytes;
        ctx->processed += info.frame_bytes;
        memmove(buf, buf + info.frame_bytes, buf_level);
        // If any samples has been decoded, mark the buffer as used
        if (samples != 0) {
            pcm_buffer_size = samples * ctx->bits_per_sample / 8 * info.channels;
            pcm_buffer_ready = 1;
        }
        else if (info.frame_bytes == 0) {
        	// nothing has been processed and not moving forward
        	ctx->processed = ctx->file_size;
        }
    }
    printf("Decoding finished.\n");
    ctx->finished = true;

    vPortFree(buf);
    vTaskSuspend(NULL);
}

// Return actual sample size filled
size_t dec_audio_callback(void *userdata, uint8_t *stream, uint32_t len) {
    DecoderContext *ctx = (DecoderContext *)userdata;

    if (!(ctx->playing)) {
        // Not playing, return silence
        memset(stream, 0, len);
        return len;
    }

    if (len < pcm_buffer_size) {
    	// Not supported
    	printf("Given buffer too small! Given %d, expected %d\n", len, pcm_buffer_size);
    	return 0;
    }

    if (pcm_buffer_ready != 1) {
        // Nothing to return for now
    	// If this is called from an ISR, this is pretty much
    	// impossible to delay it?
    	// Even if it can try later, does this mean the decoding is
    	// simply too slow and there is little we can do about that?
        return 0;
    }

    memcpy(stream, pcm_buffer, pcm_buffer_size);

    // Reset dma buffer and signal the decode thread
    size_t buf_size = pcm_buffer_size; // need to save the value before reset the variable
    pcm_buffer_ready = 0;
    xSemaphoreGiveFromISR(pcm_buffer_sem, NULL); // May need to use from ISR version
    //xTaskResumeFromISR(ctx->decode_handle);

    return buf_size;
}

int dec_openfile(DecoderContext *ctx, char *fname, FileFormat fmt) {
    if ((!ctx) || (!fname))
        return -1;
    memset(ctx, 0, sizeof(DecoderContext));

    pcm_buffer_sem = xSemaphoreCreateBinary();

    // Open file
    printf("%s\n", fname);
    ctx->file = hal_fs_open(fname, OM_READ);
    if (ctx->file == NULL) {
    	printf("Unable to open audio file.\n");
    	return -1;
    }
    ctx->file_size = hal_fs_size(ctx->file);

    // Setup queues
    ctx->input_queue = xQueueCreate(QUEUE_LENGTH, QUEUE_OBJECT);

    // Initialize status
    ctx->offset = 0;
    ctx->processed = 0;
    ctx->playing = false;
    ctx->finished = false;

    // Setup decoder
    ctx->fmt = fmt;
    if (fmt == FMT_MP3) {
        mp3dec_init(&(ctx->mp3d));
        ctx->output_format = AF_S16LE;
        ctx->bits_per_sample = 16;
        ctx->sample_rate = 44100; // TODO: Read from file
    }
    else if (fmt == FMT_WAV) {
        uint32_t dataLen;
        if (!wavhdrRead(ctx->file, &(ctx->sample_rate), &(ctx->bits_per_sample),
                &dataLen, &(ctx->channels))) {
            printf("Failed to load wave header!\n");
            return -1;
        }
        ctx->samples = dataLen / ctx->channels / (ctx->bits_per_sample / 8);
        if (ctx->bits_per_sample == 16)
            ctx->output_format = AF_S16LE;
        else
            ctx->output_format = AF_S32LE;
        printf("Sample rate: %d\nBits per sample: %d\nSamples: %d\nChannels: %d\n", ctx->sample_rate, ctx->bits_per_sample, ctx->samples, ctx->channels);
    }

    // Start threads
    xTaskCreate(dec_fetch_thread, "fetcher", 512, (void *)ctx, 4,
            &(ctx->fetch_handle));
    if (ctx->fetch_handle == NULL) {
    	printf("Unable to create task for audio file fetcher.\n");
        return -1;
    }

    // 24 KB stack for decoder as the MP3 decoder needs around 16KB stack space
    xTaskCreate(dec_decode_thread, "decoder", 6144, (void *)ctx, 4,
            &(ctx->decode_handle));
    if (ctx->decode_handle == NULL) {
    	printf("Unable to create task for audio file decoder.\n");
        return -1;
    }

    pcm_buffer_ready = 0;
    xSemaphoreGive(pcm_buffer_sem);

    return 0;
}

int dec_play(DecoderContext *ctx) {
    ctx->playing = true;
    return 0;
}

int dec_pause(DecoderContext *ctx) {
    ctx->playing = false;
    return 0;
}

int dec_close(DecoderContext *ctx) {
    vTaskDelete(ctx->fetch_handle);
    vTaskDelete(ctx->decode_handle);
    vSemaphoreDelete(pcm_buffer_sem);
    hal_fs_close(ctx->file);
    return 0;
}
