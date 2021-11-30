//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#pragma once

#include <stdio.h>
#include <stdint.h>

typedef enum {
    CS_NOT_CHARGING,
    CS_CHARGING,
    CS_FULLY_CHARGED
} ChargingStatus;

#define POWER_FULL_LEVEL 0xffff

// Initialize the power subsystem
void hal_power_init();
// Get current fuel gauge counter value
uint32_t hal_power_get_charge();
// Set current fuel gauge counter value
void hal_power_set_charge(uint32_t charge);
// Get stored empty level value
uint32_t hal_power_get_empty_level();
// Set stored empty level value
void hal_power_set_empty_level(uint32_t charge);
// Get charging status
ChargingStatus hal_power_get_charging_status();