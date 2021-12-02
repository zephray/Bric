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
#include "queue.h"
#include <stdio.h>
#include <stdbool.h>
#include "platform.h"
#include "hal_power.h"
#include "hal_input.h"
#include "ui.h"
#include "batcal.h"

#define VOLTAGE_LOW_THRESHOLD (3200)

void batcal_init() {
    uint32_t low_threshold = hal_power_get_empty_level();
    if (low_threshold == 0x00) {
        ui_message("Warning", "Battery hasn't been calibrated.\n"
                "Remaining battery level unknown.\n"
                "Consider calibrating in settings.", "OK");
        ui_wait_key_release(KEY_MASK_YES);
        ui_wait_key_press(KEY_MASK_YES);
        ui_clear();
    }
}

static bool batcal_ui(char *instruction, uint32_t lastTick, uint32_t tick, uint32_t lastCharge, uint32_t charge, uint32_t voltage) {
	char buf[256];
	TickType_t ticks = tick - lastTick;
	//int32_t chargingRate = (int32_t)((((float)charge - (float)lastCharge) * POWER_LSB) /
	//		((float) ticks / ((float)configTICK_RATE_HZ) / 3600.0f) * ((float)voltage / 1000.0f));
	/*snprintf(buf, 256,
			"%s\n"
			"Current voltage: %d mV\n"
			"Current charge: %d LSB\n"
			"Current charging rate: %d mW\n",
			instruction, voltage, charge, chargingRate);*/
	snprintf(buf, 256,
				"%s\n"
				"Current voltage: %d mV\n"
				"Current charge: %d LSB\n",
				instruction, voltage, charge);
    ui_message("Battery Calibration", buf, "Exit");
    lastTick = tick;

    uint32_t pressed_keys;
    do {
    	tick = xTaskGetTickCount();
    	pressed_keys = hal_input_get_keys();
    } while (((tick - lastTick) < pdMS_TO_TICKS(1000)) && !(pressed_keys & KEY_MASK_YES));

    if (pressed_keys & KEY_MASK_YES)
    	return false;

    return true;
}

void batcal_run() {
	TickType_t tick, lastTick;
	uint32_t charge, lastCharge;
	uint32_t voltage;
	char *instructions[] = {
			"Please connect charging cable",
			"Please wait for charging",
			"Please disconnect charging cable",
			"Please wait for discharging"
	};
	int state = 0;
	// state = 0: wait for start charging
	// state = 1: wait for charging to finish
	// state = 2: wait for start discharging
	// state = 3: wait for discharging

	tick = xTaskGetTickCount();
	charge = hal_power_get_charge();

	hal_power_set_charge(POWER_FULL_LEVEL / 2);

	while (1) {
	    lastTick = tick;
	    lastCharge = charge;
		tick = xTaskGetTickCount();
		charge = hal_power_get_charge();
		voltage = hal_power_get_voltage();
		if (batcal_ui(instructions[state], lastTick, tick, lastCharge, charge, voltage) == false)
			return;
		if (state == 0) {
			if (hal_power_get_charging_status() == CS_CHARGING) {
				state = 1;
			}
		}
		else if (state == 1) {
			if (hal_power_get_charging_status() == CS_FULLY_CHARGED) {
				hal_power_set_full_level(charge);
				state = 2;
			}
		}
		else if (state == 2) {
			if (hal_power_get_charging_status() == CS_NOT_CHARGING) {
				state = 3;
			}
		}
		else if (state == 3) {
			if (voltage <= VOLTAGE_LOW_THRESHOLD) {
				hal_power_set_empty_level(charge);
		        ui_message("Battery Calibration",
		        		"Battery has been calibrated.\n"
		        		"The device has been powered off.\n"
		        		"Please connect charging cable.", "-");
		        hal_power_shutdown();
			}
		}
	}

	ui_clear();
}
