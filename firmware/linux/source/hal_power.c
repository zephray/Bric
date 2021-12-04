//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdlib.h>
#include "hal_power.h"

static uint32_t charge_level = POWER_FULL_LEVEL;
static uint32_t empty_level = 0;
static uint32_t full_level = POWER_FULL_LEVEL;

// Initialize the power subsystem
void hal_power_init() {
    return;
}

void hal_power_shutdown() {
    printf("Shutdown\n");
    exit(0);    
}

// Get current voltage for reference
uint32_t hal_power_get_voltage() {
	return 4200;
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

// Get stored full level value
uint32_t hal_power_get_full_level() {
    return full_level;
}

// Set stored full level value
void hal_power_set_full_level(uint32_t charge) {
	full_level = charge;
}

// Get charging status
ChargingStatus hal_power_get_charging_status() {
    return CS_NOT_CHARGING;
}