//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <SDL/SDL.h>
#include "hal_audio.h"

static AudioCallback user_cb;
static int volume;

#define HAL_AUDIO_BUFFER_SIZE (8192)
uint8_t audio_buffer[HAL_AUDIO_BUFFER_SIZE];
size_t buffer_level;
AudioFormat audio_format;

int hal_audio_init() {
    // SDL should be initialized in SDL_Init called in video init
}

size_t hal_audio_callback_wrapper(void *userdata, uint8_t *stream, uint32_t len) {
    if (audio_format == AF_S32LE) {
        uint32_t *temp = malloc(len * 2);
        size_t size = user_cb(userdata, temp, len * 2);
        uint16_t *outbuf = (uint16_t *)stream;
        for (size_t i = 0; i < size / 4; i++) {
            uint32_t s32 = temp[i];
            uint16_t s16 = s32 >> 16;
            outbuf[i] = s16;
        }
        free(temp);
        return size / 2;
    }
    else {
        return user_cb(userdata, stream, len);
    }
}

void hal_audio_sdl_callback(void *userdata, uint8_t *stream, int len) {
    while (buffer_level < len) {
        buffer_level += hal_audio_callback_wrapper(userdata,
                &(audio_buffer[buffer_level]),
                HAL_AUDIO_BUFFER_SIZE - buffer_level);
    }
    SDL_MixAudio(stream, audio_buffer, len, volume);
    memmove(audio_buffer, &audio_buffer[len], buffer_level-len);
    buffer_level -= len;
}

int hal_audio_start(int samplerate, AudioFormat format, AudioCallback cb, void *userdata) {
    SDL_AudioSpec as_request;
    SDL_AudioSpec as_got;

    audio_format = format;

    as_request.freq = samplerate;
    as_request.format = AUDIO_S16;
    as_request.channels = 2;
    as_request.samples = HAL_AUDIO_BUFFER_SIZE / 4;
    as_request.callback = hal_audio_sdl_callback;
    as_request.userdata = userdata;
    if (SDL_OpenAudio(&as_request, &as_got) < 0)
    {
        fprintf(stderr, "couldn't open audio: %s\n", SDL_GetError());
        return -1;
    }
    
    user_cb = cb;
    
    buffer_level = 0;

    SDL_PauseAudio(0);

    return 0;
}

int hal_audio_stop() {
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}

// 0 - minimum (mute), 255 - maximum
int hal_audio_set_volume(uint8_t vol) {
    fprintf(stdout, "Volume set to %d\r\n", vol);
    volume = vol / 2;
}
