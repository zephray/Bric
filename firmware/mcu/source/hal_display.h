//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef __HAL_DISPLAY_H__
#define __HAL_DISPLAY_H__

#include <stdint.h>

#define DISP_WIDTH (250)
#define DISP_HEIGHT (122)

typedef enum pixel_format {
    PIXFMT_Y1,
    PIXFMT_Y2,
    PIXFMT_Y4,
    PIXFMT_Y8, 
    PIXFMT_RGB565,
    PIXFMT_RGB888,
    PIXFMT_ARGB8888,
    PIXFMT_RGBA8888
} PixelFormat;

typedef enum refersh_mode {REFRESH_FULL, REFRESH_PARTIAL} RefreshMode;

typedef struct canvas {
    uint32_t width;
    uint32_t height;
    PixelFormat pixelFormat;
    char buf[];
} Canvas;

// Power control
void hal_disp_init();
void hal_disp_deinit();

void hal_disp_enter_sleep();
void hal_disp_exit_sleep();

// Framebuffer operation
Canvas *hal_disp_create(int w, int h, PixelFormat fmt);
void hal_disp_set(Canvas *dst, int x, int y, uint32_t color);
uint32_t hal_disp_get(Canvas *src, int x, int y);
void hal_disp_fill(Canvas *dst, int x, int y, int w, int h, uint32_t color);
void hal_disp_bilt(
        Canvas *dst, Canvas *src,
        int dst_x, int dst_y, int dst_w, int dst_h,
        int src_x, int src_y, int src_w, int src_h);
void hal_disp_conv(Canvas *dst, Canvas *src);

// Commit to screen
void hal_disp_draw(Canvas *src, RefreshMode refMode);

#endif
