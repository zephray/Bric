//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include <stdio.h>
#include <stdbool.h>
#include "hal_display.h"
#include "hal_input.h"
#include "font.h"
#include "ui.h"

// Size for 8bpp FB: 29.8KB, size for 1bpp FB: 3.7KB
static Canvas *fb_mono;
static Canvas *fb_grey;

void ui_rect(int x1, int y1, int x2, int y2) {
    hal_disp_fill(fb_mono, x1, y1, 1, (y2 - y1), 0);
    hal_disp_fill(fb_mono, x1, y1, (x2 - x1), 1, 0);
    hal_disp_fill(fb_mono, x1, y2 - 1, (x2 - x1), 1, 0);
    hal_disp_fill(fb_mono, x2, y1, 1, (y2 - y1), 0);
}

void ui_init(void) {
    fb_mono = hal_disp_create(256, 128, PIXFMT_Y1);
    fb_grey = hal_disp_create(250, 122, PIXFMT_Y8);

    font_init();
    font_set_font(FNT_12);
}

void ui_message(char *title, char *text) {
    hal_disp_fill(fb_mono, 0, 0, 256, 128, 1);
    font_set_font(FNT_12);
    font_disp(fb_mono, 12, 12, 242, 0, title, 57, CE_UTF8, false);
    hal_disp_fill(fb_mono, 8, 32, 235, 1, 0);
    font_disp(fb_mono, 12, 40, 242, 0, text, 57 * 5, CE_UTF8, true);
    ui_rect(200, 98, 242, 114);
    font_disp(fb_mono, 216, 100, 40, 0, "OK", 3, CE_ASCII, false);
    hal_disp_draw(fb_mono, REFRESH_PARTIAL);
}

void ui_clear(void) {
    hal_disp_fill(fb_mono, 0, 0, 256, 128, 0);
    hal_disp_draw(fb_mono, REFRESH_PARTIAL);
}

void ui_wait_key_press(uint32_t keymask) {
    while (!(hal_input_get_keys() & keymask)) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void ui_wait_key_release(uint32_t keymask) {
    while ((hal_input_get_keys() & keymask)) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void ui_render_menu(ui_menu_t *menu, int index) {
    int render_start = 0;
    int render_length = 7;
    if ((menu->count - index) >= 4)
        render_start = index - 3;
    else
        render_start = menu->count - render_length + 1;
    if (render_start < 0) render_start = 0;
    if (menu->count < render_length)
        render_length = menu->count;

    hal_disp_fill(fb_mono, 0, 0, 256, 128, 1);
    font_set_font(FNT_12);
    font_disp(fb_mono, 12, 2, 242, 0, menu->title, 60, CE_UTF8, false);
    hal_disp_fill(fb_mono, 8, 22, 235, 1, 0);

    int inv_offset = index - render_start;
    hal_disp_fill(fb_mono, 4, 30 + inv_offset * 13, 242, 13, 0);

    for (int i = render_start; i < (render_start + render_length); i++) {
        font_disp(fb_mono, 8, 30 + (i - render_start) * 13, 240,
                (i == index) ? 1 : 0, menu->items[i].title, 60, CE_UTF8, false);
    }

    hal_disp_draw(fb_mono, REFRESH_PARTIAL);
}

int ui_run_menu(ui_menu_t *menu, int index) {
    uint32_t key_state;
    
    while (1) {
        ui_render_menu(menu, index);
        ui_wait_key_release(0xff);
        ui_wait_key_press(0xff);
        key_state = hal_input_get_keys();
        if (key_state & KEY_MASK_UP) {
            if (index > 0) index--;
        }
        if (key_state & KEY_MASK_DOWN) {
            if (index < menu->count - 1) index++;
        }
        if (key_state & KEY_MASK_YES) {
            return index;
        }
        if (key_state & KEY_MASK_NO) {
            return -1;
        }
    }
}
