//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "FreeRTOS.h"
#include <stdio.h>
#include <stdbool.h>
#include "hal_display.h"
#include "epd.h"

static RefreshMode curMode;

// Power control
void hal_disp_init() {
	EPD_Init();
	EPD_Clear();
	curMode = REFRESH_FULL;
}

void hal_disp_deinit() {
    // TODO
}

void hal_disp_enter_sleep() {
    printf("hal_disp: enter sleep\n");
}

void hal_disp_exit_sleep() {
    printf("hal_disp: exit sleep\n");
}

static int hal_disp_get_bpp(PixelFormat fmt) {
    switch(fmt) {
    case PIXFMT_Y1:
        return 1;
    case PIXFMT_Y2:
        return 2;
    case PIXFMT_Y4:
        return 4;
    case PIXFMT_Y8:
        return 8;
    case PIXFMT_RGB565:
        return 16;
    case PIXFMT_RGB888:
        return 24;
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        return 32;
    }

    return -1;
}

static uint8_t hal_disp_get_mask(PixelFormat fmt) {
    switch(fmt) {
    case PIXFMT_Y1:
        return 0x01;
    case PIXFMT_Y2:
        return 0x03;
    case PIXFMT_Y4:
        return 0x0f;
    default:
        return 0x00; // not applicable
    }
}

// Framebuffer operation
Canvas *hal_disp_create(int w, int h, PixelFormat fmt) {
    size_t size = w * h;
    int bpp = hal_disp_get_bpp(fmt);
    if (bpp >= 8) {
        size *= (bpp / 8);
    }
    else {
        size /= (8 / bpp);
    }
    Canvas *canvas = pvPortMalloc(sizeof(Canvas) + size);
    canvas->width = w;
    canvas->height = h;
    canvas->pixelFormat = fmt;
    return canvas;
}

void hal_disp_free(Canvas *canvas) {
	vPortFree(canvas);
}

void hal_disp_set(
        Canvas *dst, int x, int y, uint32_t color) {
    int bpp;
    int pbb;
    int sft;
    uint8_t mask;
    size_t offset;

    if ((x >= dst->width) || (y >= dst->height))
    	return;

    switch(dst->pixelFormat) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
        bpp = hal_disp_get_bpp(dst->pixelFormat);
        pbb = 8 / bpp;
        sft = pbb - y % pbb - 1;
        sft *= bpp;
        mask = hal_disp_get_mask(dst->pixelFormat);
        offset = x * dst->height / pbb + y / pbb;
        dst->buf[offset] &= ~(mask << sft);
        dst->buf[offset] |= (uint8_t)color << sft;
        break;
    case PIXFMT_Y8:
        offset = x * dst->width + y;
        dst->buf[offset] = (uint8_t)color;
        break;
    case PIXFMT_RGB565:

        offset = (y * dst->width + x) * 2;
        *(uint16_t *)(&(dst->buf[offset])) = (uint16_t)color;
        break;
    case PIXFMT_RGB888:
        offset = (y * dst->width + x) * 3;
        dst->buf[offset] = (uint8_t)(color & 0xff);
        dst->buf[offset+1] = (uint8_t)((color >> 16) & 0xff);
        dst->buf[offset+2] = (uint8_t)((color >> 24) & 0xff);
        break;
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        offset = (y * dst->width + x) * 4;
        *(uint32_t *)(&(dst->buf[offset])) = color;
        break;
    }
}

uint8_t hal_disp_pix_to_y(uint32_t color, PixelFormat fmt) {
    uint8_t y = (uint32_t)color;
    switch (fmt) {
    case PIXFMT_Y1:
        y = (color) ? 0xff : 0x00;
        break;
    case PIXFMT_Y2:
        y |= y << 2; __attribute__ ((fallthrough));
    case PIXFMT_Y4:
        y |= y << 4; __attribute__ ((fallthrough));
    case PIXFMT_Y8:
        break;
    default:
        fprintf(stderr, "Converting from color to Y\n");
        break;
    }
    return y;
}

uint32_t hal_disp_get(Canvas *src, int x, int y) {
    int bpp;
    int pbb;
    int sft;
    uint8_t mask;
    size_t offset;
    uint32_t color;

    switch(src->pixelFormat) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
        bpp = hal_disp_get_bpp(src->pixelFormat);
        pbb = 8 / bpp;
        sft = y % pbb;
        mask = hal_disp_get_mask(src->pixelFormat);
        offset = x * src->width / pbb + y / pbb;
        color = src->buf[offset] >> sft;
        return (uint32_t)(color & mask);
        break;
    case PIXFMT_Y8:
        offset = x * src->width + y;
        return (uint32_t)src->buf[offset];
        break;
    case PIXFMT_RGB565:
    case PIXFMT_RGB888:
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        fprintf(stderr, "Unimplemented function: Get pixel in color buffer\n");
        break;
    }
    return 0;
}

void hal_disp_fill(Canvas *dst, int x, int y, int w, int h, uint32_t color) {
    int bpp;
    int pbb;
    size_t offset;

    switch(dst->pixelFormat) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
        bpp = hal_disp_get_bpp(dst->pixelFormat);
        pbb = 8 / bpp;
        if ((y % pbb == 0) && (h % pbb == 0)) {
            // Aligned, do byte operation directly
            uint8_t colorfill = hal_disp_pix_to_y(color, dst->pixelFormat);
            for (int xx = x; xx < x + w; xx++) {
                offset = (xx * dst->height + y) / pbb;
                for (int yy = 0; yy < (h / pbb); yy++)
                    dst->buf[offset++] = colorfill;
            }
        }
        else {
            // Not aligned, fallback to using setpixel
            for (int yy = y; yy < y + h; yy++) {
                for (int xx = x; xx < x + w; xx++)
                    hal_disp_set(dst, xx, yy, color);
            }
        }
        break;
    case PIXFMT_Y8:
        for (int xx = x; xx < x + w; xx++) {
            offset = xx * dst->height + y;
            for (int yy = 0; yy < h; yy++)
                dst->buf[offset++] = (uint8_t)color;
        }
        break;
    case PIXFMT_RGB565:
    case PIXFMT_RGB888:
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        fprintf(stderr, "Unimplemented function: Fill color buffer\n");
        break;
    }
}

void hal_disp_inv(Canvas *dst, int x, int y, int w, int h) {
    int bpp;
    int pbb;
    size_t offset;
    uint32_t color;

    switch(dst->pixelFormat) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
        bpp = hal_disp_get_bpp(dst->pixelFormat);
        pbb = 8 / bpp;
        if ((y % pbb == 0) && (h % pbb == 0)) {
            // Aligned, do byte operation directly
            for (int xx = x; xx < x + w; xx++) {
                offset = (xx * dst->height + y) / pbb;
                for (int yy = 0; yy < (h / pbb); yy++) {
                    color = dst->buf[offset];
                    dst->buf[offset++] = ~color;
                }
            }
        }
        else {
            // Not aligned, fallback to using setpixel
            for (int yy = y; yy < y + h; yy++) {
                for (int xx = x; xx < x + w; xx++)
                    hal_disp_set(dst, xx, yy, !hal_disp_get(dst, xx, yy));
            }
        }
        break;
    case PIXFMT_Y8:
        for (int xx = x; xx < x + w; xx++) {
            offset = xx * dst->width + y;
            for (int yy = 0; yy < h; yy++) {
                color = dst->buf[offset];
                dst->buf[offset++] = 255 - color;
            }
        }
        break;
    case PIXFMT_RGB565:
    case PIXFMT_RGB888:
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        fprintf(stderr, "Unimplemented function: Fill color buffer\n");
        break;
    }
}

void hal_disp_bilt(
        Canvas *dst,
        Canvas *src,
        int dst_x, int dst_y, int dst_w, int dst_h,
        int src_x, int src_y, int src_w, int src_h) {

}

uint32_t hal_disp_conv_pix(PixelFormat dst, PixelFormat src, uint32_t color) {
    int r = 0x00, g = 0x00, b = 0x00, a = 0xff;
    switch (src) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
    case PIXFMT_Y8:
        r = g = b = hal_disp_pix_to_y(color, src);
        break;
    case PIXFMT_RGB565:
        r = (color >> 8) & 0xf8;
        g = (color >> 3) & 0xfc;
        b = (color << 3) & 0xf8;
        r |= r >> 5;
        g |= g >> 6;
        b |= b >> 5;
        break;
    case PIXFMT_ARGB8888:
        a = (color >> 24) & 0xff; __attribute__ ((fallthrough));
    case PIXFMT_RGB888:
        r = (color >> 16) & 0xff;
        g = (color >> 8) & 0xff;
        b = (color) & 0xff;
        break;
    case PIXFMT_RGBA8888:
        r = (color >> 24) & 0xff;
        g = (color >> 16) & 0xff;
        b = (color >> 8) & 0xff;
        a = (color) & 0xff;
        break;
    }

    uint8_t y = (uint8_t)((r + g + b) / 3);
    uint32_t target = 0;
    switch (dst) {
    case PIXFMT_Y1:
        target = (y >> 7) & 0x1;
        break;
    case PIXFMT_Y2:
        target = (y >> 6) & 0x3;
        break;
    case PIXFMT_Y4:
        target = (y >> 4) & 0xf;
        break;
    case PIXFMT_Y8:
        target = y;
        break;
    case PIXFMT_RGB565:
        target = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
        break;
    case PIXFMT_ARGB8888:
        target = (a << 24) | (r << 16) | (g << 8) | b;
        break;
    case PIXFMT_RGB888:
        target = (r << 16) | (g << 8) | b;
        break;
    case PIXFMT_RGBA8888:
        target = (r << 24) | (g << 16) | (b << 8) | a;
        break;
    }
    return target;
}

void hal_disp_conv(Canvas *dst, Canvas *src) {

}

// Commit to screen
void hal_disp_draw(Canvas *src, RefreshMode refMode) {
	if (refMode != curMode) {
		if (refMode == REFRESH_PARTIAL)
			EPD_SwitchToPartial();
		curMode = refMode;
	}

	if (src->pixelFormat == PIXFMT_Y1) {
		EPD_DispImg((uint8_t *)src->buf, (refMode == REFRESH_PARTIAL));
	}
	else if (src->pixelFormat == PIXFMT_Y8) {
		EPD_DispGreyscale((uint8_t *)src->buf);
		EPD_ClearBuffer(); // It is necessary to clear internal state after using Greyscale mode
	}
	else {
		// TODO
	}
}
