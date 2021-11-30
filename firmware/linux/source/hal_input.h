//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#pragma once

#include <SDL/SDL.h>

//#define KEY_MASK_LEFT   (1 << 0)
//#define KEY_MASK_RIGHT  (1 << 1)
#define KEY_MASK_UP     (1 << 2)
#define KEY_MASK_DOWN   (1 << 3)
#define KEY_MASK_YES    (1 << 4)
#define KEY_MASK_NO     (1 << 5)
//#define KEY_MASK_START  (1 << 6)
//#define KEY_MASK_SELECT (1 << 7)

void hal_input_init(void);
uint32_t hal_input_get_keys(void);
void _hal_input_handle_sdl_keyevent(SDL_KeyboardEvent event);
