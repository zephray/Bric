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

extern QueueHandle_t g_powerButtonEventQueue;

// mAh per LSB
#define POWER_LSB (0.085f * 16.f / 128.f)

// Initialize the power subsystem
void hal_power_init();
// Get current voltage for reference
uint32_t hal_power_get_voltage();
// Get current fuel gauge counter value
uint32_t hal_power_get_charge();
// Set current fuel gauge counter value
void hal_power_set_charge(uint32_t charge);
// Get stored empty level value
uint32_t hal_power_get_empty_level();
// Set stored empty level value
void hal_power_set_empty_level(uint32_t charge);
// Get stored full level value
uint32_t hal_power_get_full_level();
// Set stored full level value
void hal_power_set_full_level(uint32_t charge);
// Get charging status
ChargingStatus hal_power_get_charging_status();
// Shutdown the system
void hal_power_shutdown();
//
bool hal_power_is_power_button_pressed(void);
