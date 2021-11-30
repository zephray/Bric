//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#pragma once

#define KEY_SHIFT_LEFT   (0)
#define KEY_SHIFT_RIGHT  (1)
#define KEY_SHIFT_UP     (2)
#define KEY_SHIFT_DOWN   (3)
#define KEY_SHIFT_YES    (4)
#define KEY_SHIFT_NO     (5)
#define KEY_SHIFT_START  (6)
#define KEY_SHIFT_SELECT (7)

#define KEY_MASK_LEFT   (1 << KEY_SHIFT_LEFT)
#define KEY_MASK_RIGHT  (1 << KEY_SHIFT_RIGHT)
#define KEY_MASK_UP     (1 << KEY_SHIFT_UP)
#define KEY_MASK_DOWN   (1 << KEY_SHIFT_DOWN)
#define KEY_MASK_YES    (1 << KEY_SHIFT_YES)
#define KEY_MASK_NO     (1 << KEY_SHIFT_NO)
#define KEY_MASK_START  (1 << KEY_SHIFT_START)
#define KEY_MASK_SELECT (1 << KEY_SHIFT_SELECT)

void hal_input_init(void);
uint32_t hal_input_get_keys(void);
void hal_input_scan(void);
