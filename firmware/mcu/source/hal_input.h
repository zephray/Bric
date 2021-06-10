//
// Project Fobu
// Copyright 2020 Wenting Zhang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// File : os_input.h
// Brief: Keypad input OS Abstraction Layer
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
