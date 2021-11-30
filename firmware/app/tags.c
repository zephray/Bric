//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>
#include "hal_filesystem.h"
#include "tjpgd.h"
#include "id3v2lib/id3v2lib.h"
#include "tags.h"

static float lerp(float s, float e, float t) {
    return s + (e - s) * t;
}

static float blerp(float c00, float c10, float c01, float c11, float tx, float ty) {
    return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

// Context data structure for JPEG decoding callback
typedef struct {
    File *fp;           // file pointer of the input file
    uint8_t *fbuf;      // pointer to the output framebuffer
    uint8_t cbuf[32];   // Column buffer for block bilinear scaling
    uint8_t dw_buf[32]; // deferred line buffer writing content
    uint32_t dw_count;  // deferred line buffer writing count
    uint32_t dw_offset; // deferred line buffer writing offset
    uint8_t *lbuf;      // Line buffer
    size_t stride;          // Width of the frame buffer (>= dst_width)
    int src_width;      // Source image width
    int src_height;     // Source image height
    int dst_width;      // Destination image width
    int dst_height;     // Destination image height
    int factor;         // Prescaling factor
} IODEV;

uint16_t in_func (JDEC* jd, uint8_t* buff, uint16_t nbyte)
{
    IODEV *dev = (IODEV*)jd->device;   /* Device identifier for the session (5th argument of jd_prepare function) */
    uint32_t bytes;

    if (buff) {
        /* Read bytes from input stream */
        if ((bytes = hal_fs_read(dev->fp, buff, nbyte)) < 0) {
        	bytes = 0;
        }
    } else {
        /* Remove bytes from input stream */
        hal_fs_seek(dev->fp, hal_fs_tell(dev->fp) + nbyte);
        bytes = nbyte;
    }
    return bytes;
}

// Helper functions for bilinear interpolation
static uint8_t getpixel_from_linebuffer(uint8_t *buf, int x) {
    return buf[x];
}

static uint8_t getpixel_from_columnbuffer(uint8_t *buf, int y) {
    return buf[y];
}

static uint8_t getpixel_from_bitmap(uint8_t *buf, int width, int x, int y) {
    return buf[y*width + x];
}

static uint8_t getpixel(IODEV *dev, uint8_t *bitmap, JRECT *rect, int x, int y) {
    if (y == -1) {
        // Line buffer uses global X position
        return getpixel_from_linebuffer(dev->lbuf, x + rect->left);
    }
    else if (x == -1) {
        // Column buffer uses local Y position
        return getpixel_from_columnbuffer(dev->cbuf, y);
    }
    else {
        return getpixel_from_bitmap(bitmap, rect->right - rect->left + 1, x, y);
    }
}

static void setpixel(uint8_t *fbuf, int width, int x, int y, uint8_t c) {
    //printf("setpix(%d, %d)\n", x, y);
    fbuf[y * width + x] = c;
}

static void output_bilinear(IODEV *dev, void* bitmap, JRECT *rect) {

    float xa = (float)(rect->left) * (float)(dev->dst_width) / (dev->src_width - 1);
    float ya = (float)(rect->top) * (float)(dev->dst_height) / (dev->src_height - 1);
    float xb = (float)(rect->right + 1) * (float)(dev->dst_width) / (dev->src_width - 1);
    float yb = (float)(rect->bottom + 1) * (float)(dev->dst_height) / (dev->src_height - 1);

    int xai = (int)xa - 1;
    int yai = (int)ya - 1;
    int xbi = (int)xb;
    int ybi = (int)yb;
    if (xai < 0) xai = 0; // clamp
    if (yai < 0) yai = 0; // clamp

    int rw = rect->right - rect->left + 1;
    int rh = rect->bottom - rect->top + 1;

    for (int y = yai; y <= ybi; y++) {
        for (int x = xai; x <= xbi; x++) {
            float gx = x / (float)(dev->dst_width) * (dev->src_width - 1);
            float gy = y / (float)(dev->dst_height) * (dev->src_height - 1);
            int gxi = (int)gx;
            int gyi = (int)gy;
            int gxr = gxi - rect->left;
            int gyr = gyi - rect->top;
            if (gxr < -1)
                continue;
            if (gyr < -1)
                continue;
            if (gxr + 1 >= rw)
                continue;
            if (gyr + 1 >= rh)
                continue;
            uint8_t c00, c10, c01, c11;
            c00 = getpixel(dev, bitmap, rect, gxr, gyr);
            c10 = getpixel(dev, bitmap, rect, gxr+1, gyr);
            c01 = getpixel(dev, bitmap, rect, gxr, gyr+1);
            c11 = getpixel(dev, bitmap, rect, gxr+1, gyr+1);
            uint8_t result = (uint8_t)blerp(c00, c10, c01, c11, gx - gxi, gy - gyi);

            result = 255 - result;
            setpixel(dev->fbuf, dev->stride, x, y, result);
        }
    }

    for (int y = 0; y < rh; y++) {
        dev->cbuf[y] = getpixel_from_bitmap(bitmap, rw, rw - 1, y);
    }
    for (int x = 0; x < dev->dw_count; x++) {
        dev->lbuf[dev->dw_offset + x] = dev->dw_buf[x];
    }
    dev->dw_offset = rect->left;
    dev->dw_count = rw;
    for (int x = 0; x < dev->dw_count; x++) {
        dev->dw_buf[x] = getpixel_from_bitmap(bitmap, rw, x, rh - 1);
    }
}

static uint16_t out_func (JDEC* jd, void* bitmap, JRECT* rect)
{
    IODEV *dev = (IODEV*)jd->device;
    int rw = rect->right - rect->left + 1;
    int rh = rect->bottom - rect->top + 1;
    int factor = dev->factor;

    for (int j = 0; j < rh / factor; j++) {
        for (int i = 0; i < rw / factor; i++) {
            uint32_t acc = 0;
            for (int y = 0; y < factor; y++) {
                for (int x = 0; x < factor; x++) {
                    acc += getpixel_from_bitmap(bitmap, rw, i*factor+x, j*factor+y);
                }
            }
            acc /= (factor * factor);
            ((uint8_t *)bitmap)[j * (rh / factor) + i] = (uint8_t)acc;
        }
    }

    rect->left /= factor;
    rect->right = (rect->right + 1) / factor - 1;
    rect->top /= factor;
    rect->bottom = (rect->bottom + 1) / factor - 1;
    output_bilinear(dev, bitmap, rect);

    return 1;    /* Continue to decompress */
}

static ID3v2_tag *tags;

int tags_open(char *fn) {
	tags = load_tag(fn);
	if (!tags)
		return -1;

	return 0;
}

int tags_display_cover(uint8_t *destination, size_t stride, int width, int height) {
	if (!tags)
		return -1;

	ID3v2_frame *frame = tag_get_album_cover(tags);
	if (!frame)
		return -1;

	ID3v2_frame_apic_content *apic = parse_apic_frame_content(tags->file, frame);
	if (!apic)
		return -1;

    void *work;
    JDEC jdec;
    JRESULT res;
    IODEV devid;

    devid.fp = tags->file;

    hal_fs_seek(devid.fp, apic->offset);

	// Allocate a work area for TJpgDec
	work = pvPortMalloc(3100);
	if (!work)
		goto error;

	// Prepare to decompress
	res = jd_prepare(&jdec, in_func, work, 3100, &devid);
	if (res == JDR_OK) {
		/* Ready to dcompress. Image info is available here. */
		printf("Image dimensions: %u by %u. %u bytes used.\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);

		int dim = (jdec.width < jdec.height) ? (jdec.width) : (jdec.height);
		if (dim <= height * 2) {
			devid.factor = 1;
		}
		else if (dim <= height * 4) {
			devid.factor = 2;
		}
		else if (dim <= height * 8) {
			devid.factor = 4;
		}
		else {
			devid.factor = 8;
		}

		devid.src_width = jdec.width / devid.factor;
		devid.src_height = jdec.height / devid.factor;
		devid.dst_width = width;
		devid.dst_height = height;
		devid.fbuf = destination;
		devid.lbuf = pvPortMalloc(devid.src_width);
		devid.dw_count = 0;
		devid.dw_offset = 0;
		devid.stride = stride;

		printf("Factor %d, Prescaled to %d x %d\n", devid.factor, devid.src_width, devid.src_height);

		res = jd_decomp(&jdec, out_func, 0);   // Start to decompress
		if (res != JDR_OK) {
			printf("Failed to decompress: rc=%d\n", res);
			goto error;
		}
		vPortFree(devid.lbuf);
	} else {
		printf("Failed to prepare: rc=%d\n", res);
		goto error;
	}


	vPortFree(work);
	free_apic_content(apic);

	return 0;

error:
	if (work)
		vPortFree(work);
	if (devid.lbuf)
		vPortFree(devid.lbuf);
	if (apic)
		free_apic_content(apic);
	return -1;
}

static char *tags_get_string_content(ID3v2_frame *frame) {
	ID3v2_frame_text_content *content = parse_text_frame_content(tags->file, frame);
	if (!content)
		return NULL;

	char *res = (char *)pvPortMalloc(content->size + 2);
	memcpy(res + 1, content->data, content->size);
	res[0] = content->encoding; // Prefix
	res[content->size + 1] = 0; // Terminator

	free_text_content(content);

	return res;
}

char *tags_get_title() {
	ID3v2_frame *frame = tag_get_title(tags);
	if (!frame)
		return NULL;
	return tags_get_string_content(frame);
}

char *tags_get_album() {
	ID3v2_frame *frame = tag_get_album(tags);
	if (!frame)
		return NULL;
	return tags_get_string_content(frame);
}

char *tags_get_artist() {
	ID3v2_frame *frame = tag_get_artist(tags);
	if (!frame)
		return NULL;
	return tags_get_string_content(frame);
}

void tags_close() {
    hal_fs_close(tags->file);
	free_tag(tags);
}
