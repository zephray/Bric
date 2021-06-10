//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include "fsl_pint.h"
#include "fsl_common.h"
#include "fsl_inputmux.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "usb.h"
#include "pm.h"
#include "ltc2942.h"

#define PINT_CHARGE_SRC kINPUTMUX_GpioPort1Pin2ToPintsel
#define PINT_PBSTAT_SRC kINPUTMUX_GpioPort1Pin3ToPintsel

QueueHandle_t g_powerButtonEventQueue;

void pm_charge_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    uint32_t nchrg = GPIO_PinRead(GPIO, BOARD_INITPINS_CHRG_PORT, BOARD_INITPINS_CHRG_PIN);
    printf("\f\r\nPINT Charge Interrupt %d event detected, status = %d.", pintr, nchrg);
    if (nchrg == 1) {
    	// Charging stopped
    	GPIO_PortClear(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);
    	if (USBHSD->DEVCMDSTAT & (1ul << 28)) {
    		// VBus detected, charge finished
    		printf("VBus detected, charge finished\n");
    		// Is it possible that VBUS is present but because of poor power supply the battery is not charging?
    	}
    	else {
    		// VBus not detected, charge interrupted
    		printf("VBus not detected, charge interrupted\n");
    	}
    }
    else {
        // Charging started
    	GPIO_PortSet(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);
    	if (USBHSD->DEVCMDSTAT & (1ul << 28)) {
			// VBus detected, charge started
			printf("VBus detected, charge started\n");
		}
		else {
			// VBus not detected, how does charge start?
			printf("VBus not detected, how is this even possible?\n");
		}
    }
}

void pm_pbstat_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    bool pbstat = pm_is_power_button_pressed();
	printf("\f\r\nPINT Pbstat Interrupt %d event detected, status = %d.", pintr, pbstat);
	xQueueSendFromISR(g_powerButtonEventQueue, &pbstat, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void pm_init() {
    LTC2942_Init();

    g_powerButtonEventQueue = xQueueCreate(2, sizeof(bool));

	INPUTMUX_Init(INPUTMUX);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, PINT_CHARGE_SRC);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt1, PINT_PBSTAT_SRC);
	// Deactivate inputmux clock to save power
	INPUTMUX_Deinit(INPUTMUX);

	GPIO_PortClear(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);

	// Update charging status
	pm_charge_callback(kPINT_PinInt0, 0);

	// Initialize PINT
	PINT_Init(PINT);
	PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableBothEdges, pm_charge_callback);
	PINT_PinInterruptConfig(PINT, kPINT_PinInt1, kPINT_PinIntEnableBothEdges, pm_pbstat_callback);
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt1);

	// Initialize USB device controller here so VBUS status can be read. This should be removed once USB driver is in
	CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_UsbPhySrcExt, 16000000U);
	CLOCK_EnableUsbhs0DeviceClock(kCLOCK_UsbSrcUnused, 0U);
	USB_EhciPhyInit(kUSB_ControllerLpcIp3511Hs0, 16000000U, NULL);
}

bool pm_is_power_button_pressed(void) {
    uint32_t pbstat = GPIO_PinRead(GPIO, BOARD_INITPINS_PB_STAT_PORT, BOARD_INITPINS_PB_STAT_PIN);
    return !pbstat;
}

void pm_shutdown() {
    LTC2942_Shutdown();
    GPIO_PortClear(GPIO, BOARD_INITPINS_PMIC_ON_REQ_PORT, 1u << BOARD_INITPINS_PMIC_ON_REQ_PIN);
}
