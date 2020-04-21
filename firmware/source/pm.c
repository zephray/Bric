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

#define PINT_CHARGE_SRC kINPUTMUX_GpioPort1Pin2ToPintsel
#define PINT_PBSTAT_SRC kINPUTMUX_GpioPort1Pin3ToPintsel

void pm_charge_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    printf("\f\r\nPINT Charge Interrupt %d event detected, status = %d.", pintr, pmatch_status);
    if (GPIO_PinRead(GPIO, BOARD_INITPINS_LED_PORT, BOARD_INITPINS_LED_PIN)) {
    	// High, not charging
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
    	GPIO_PortSet(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);
    	if (USBHSD->DEVCMDSTAT & (1ul << 28)) {
			// VBus detected, charge finished
			printf("VBus detected, charge started\n");
			// Is it possible that VBUS is present but because of poor power supply the battery is not charging?
		}
		else {
			// VBus not detected, charge interrupted
			printf("VBus not detected, how is this even possible?\n");
		}
    }
}

void pm_pbstat_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
	printf("\f\r\nPINT Pbstat Interrupt %d event detected, status = %d.", pintr, pmatch_status);
}

void pm_init() {
	INPUTMUX_Init(INPUTMUX);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, PINT_CHARGE_SRC);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt1, PINT_PBSTAT_SRC);
	// Deactivate inputmux clock to save power
	INPUTMUX_Deinit(INPUTMUX);

	// Initialize PINT
	PINT_Init(PINT);
	PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableBothEdges, pm_charge_callback);
	PINT_PinInterruptConfig(PINT, kPINT_PinInt1, kPINT_PinIntEnableFallEdge, pm_pbstat_callback);
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt1);

	GPIO_PortClear(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);

	// Initialize USB device controller here so VBUS status can be read. This should be removed once USB driver is in
	CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_UsbPhySrcExt, 16000000U);
	CLOCK_EnableUsbhs0DeviceClock(kCLOCK_UsbSrcUnused, 0U);
	USB_EhciPhyInit(kUSB_ControllerLpcIp3511Hs0, 16000000U, NULL);
}
