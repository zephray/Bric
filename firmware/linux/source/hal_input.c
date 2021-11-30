//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <SDL/SDL.h>
#include "hal_input.h"

static  uint32_t key_state;

void hal_input_init(void) {
    key_state = 0;
}

uint32_t hal_input_get_keys(void) {
    return key_state;
}

void _hal_input_handle_sdl_keyevent(SDL_KeyboardEvent event) {
    uint32_t key_mask;

    switch (event.keysym.sym) {
    /*case SDLK_LEFT:
        key_mask = KEY_MASK_LEFT;
        break;
    case SDLK_RIGHT:
        key_mask = KEY_MASK_RIGHT;
        break;*/
    case SDLK_UP:
        key_mask = KEY_MASK_UP;
        break;
    case SDLK_DOWN:
        key_mask = KEY_MASK_DOWN;
        break;
    case SDLK_RETURN:
        key_mask = KEY_MASK_YES;
        break;
    case SDLK_ESCAPE:
        key_mask = KEY_MASK_NO;
        break;
    /*case SDLK_z:
        key_mask = KEY_MASK_SELECT;
        break;
    case SDLK_x:
        key_mask = KEY_MASK_START;
        break;*/
    default:
        // Ignore
        return;
    }

    if (event.type == SDL_KEYDOWN) {
        key_state |= key_mask;
    }
    else {
        key_state &= ~key_mask;
    }
}