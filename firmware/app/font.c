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
#include "platform.h"
#include "hal_filesystem.h"
#include "hal_display.h"
#include "font.h"
#include "pcf.h"

static pf font_pf;

void font_init() {
    font_pf.init = 0;
    pcf_open(&font_pf, PCF_FONT_PATH);
}

void font_draw(Canvas *canvas, int x, int y, uint32_t color, uint32_t unicode, int *w, int *h) {
	bitmap b;
	metric_entry me;
	pcf_lookup(&font_pf, unicode, &b, &me);
	for (int i = 0, stride = 4; i < b.length; i += stride) {
		for (int j = 0; j < stride; j++) {
			char bits = b.data[i + j];
			for (int k = 7; k >= 0; k--) {
				if (j * 8 + 7 - k > me.character_width) { continue; }
				if ((bits << k) & 0x80) {
                    hal_disp_set(canvas, x+j*8+k, y, color);
				}
			}
		}
        y++;
	}
	pcf_bitmap_free(&b);
    *w = me.character_width;
    *h = b.length / 4;
}

uint32_t font_decode(char *c, CharEncoding encoding, int *size) {
    // todo: decode encoded char from string, return the UCS value, and set the size to the byte length of the encoded char
    uint32_t ucf = 0xfffd;
    uint8_t ch = *c++;
    switch (encoding) {
    case CE_ASCII:
        ucf = ch & 0xff;
        *size = 1;
        break;
    case CE_UCS2:
    	ucf = ch & 0xff;
    	ucf |= ((*c) << 8) & 0xff00;
    	*size = 2;
    	break;
    case CE_UTF8:
    	if ((ch & 0x80) == 0x00) {
    		ucf = ch & 0xff;
    		*size = 1;
    	}
    	else if ((ch & 0xe0) == 0xc0) {
    		ucf = (ch & 0x1f) << 6;
    		ucf |= (*c++ & 0x3f);
    		*size = 2;
    	}
    	else if ((ch & 0xf0) == 0xe0) {
    		ucf = (ch & 0x0f) << 12;
    		ucf |= (*c++ & 0x3f) << 6;
    		ucf |= (*c++ & 0x3f);
    		*size = 3;
    	}
    	else if ((ch & 0xf8) == 0xf0) {
			ucf = (ch & 0x0f) << 18;
			ucf |= (*c++ & 0x3f) << 12;
			ucf |= (*c++ & 0x3f) << 6;
			ucf |= (*c++ & 0x3f);
			*size = 4;
		}
    	else {
    		*size = 1;
    	}
    	break;
    default:
        fprintf(stderr, "Encoding not supported yet!\n");
        break;
    }
    return ucf;
}

void font_disp(Canvas *canvas, int x, int y, uint32_t color, char *string, int len, CharEncoding encoding) {
    // Dangerous
    int count = 0, char_size = 0;
    while ((count < len) && (string[count])) {
        uint32_t ucf = font_decode(&(string[count]), encoding, &char_size);
        count += char_size;
        int w, h;
        font_draw(canvas, x, y, color, ucf, &w, &h);
        x += w;
    }
}
