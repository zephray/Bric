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
// File : os_input.c
// Brief: Keypad input OS Abstraction Layer
//
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include <stdio.h>
#include <stdbool.h>
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "hal_input.h"

static uint32_t key_state;

void hal_input_init(void) {
    key_state = 0;
}

uint32_t hal_input_get_keys(void) {
    return key_state;
}

static uint32_t hal_input_get_keys_raw(void) {
    uint32_t key_state = 0;
    key_state |= (!GPIO_PinRead(GPIO, BOARD_INITPINS_SW_SEL_PORT,
            BOARD_INITPINS_SW_SEL_PIN)) << KEY_SHIFT_YES;
    key_state |= (!GPIO_PinRead(GPIO, BOARD_INITPINS_SW_PLUS_PORT,
            BOARD_INITPINS_SW_PLUS_PIN)) << KEY_SHIFT_UP;
    key_state |= (!GPIO_PinRead(GPIO, BOARD_INITPINS_SW_MIN_PORT,
            BOARD_INITPINS_SW_MIN_PIN)) << KEY_SHIFT_DOWN;
    key_state |= (!GPIO_PinRead(GPIO, BOARD_INITPINS_SW_BACK_PORT,
            BOARD_INITPINS_SW_BACK_PIN)) << KEY_SHIFT_NO;
    return key_state;
}

// Function to be called in background task
void hal_input_scan(void) {
    uint32_t state_1, state_2;
    state_1 = hal_input_get_keys_raw();
    if (state_1 ^ key_state) {
        // If the state is different
        // Debouncing
        vTaskDelay(pdMS_TO_TICKS(20));
        state_2 = hal_input_get_keys_raw();
        key_state = state_1 & state_2;
    }
}
