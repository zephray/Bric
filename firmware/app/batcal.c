//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//

// TODO: This should really be calling some HAL
#include "FreeRTOS.h"
#include <stdio.h>
#include <stdbool.h>
#include "platform.h"
#include "ltc2942.h"
#include "hal_input.h"
#include "ui.h"
#include "batcal.h"

void batcal_init() {
    uint32_t low_threshold = LTC2942_GetLowThreshold();
    if (low_threshold == 0x00) {
        ui_message("Warning", "Battery hasn't been calibrated.\n"
                "Remaining battery level unknown.\n"
                "Consider calibrating in settings.");
        ui_wait_key_release(KEY_MASK_YES);
        ui_wait_key_press(KEY_MASK_YES);
        ui_clear();
    }
}
