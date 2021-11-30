//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "hal_power.h"

static uint32_t charge_level = POWER_FULL_LEVEL;
static uint32_t empty_level = 0;

// Initialize the power subsystem
void hal_power_init() {
    return;
}

// Get current fuel gauge counter value
uint32_t hal_power_get_charge() {
    return charge_level;
}

// Set current fuel gauge counter value
void hal_power_set_charge(uint32_t charge) {
    if (charge < POWER_FULL_LEVEL)
        charge_level = charge;
}

// Get stored empty level value
uint32_t hal_power_get_empty_level() {
    return empty_level;
}

// Set stored empty level value
void hal_power_set_empty_level(uint32_t charge) {
    if (charge < POWER_FULL_LEVEL)
        empty_level = charge;
}

// Get charging status
ChargingStatus hal_power_get_charging_status() {
    return CS_NOT_CHARGING;
}