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
#include "fsl_power.h"
#include "pin_mux.h"
#include "usb.h"
#include "ltc2942.h"
#include "hal_power.h"

#define PINT_CHARGE_SRC kINPUTMUX_GpioPort1Pin2ToPintsel
#define PINT_PBSTAT_SRC kINPUTMUX_GpioPort1Pin3ToPintsel

QueueHandle_t g_powerButtonEventQueue;
static volatile ChargingStatus cs;

static void hal_power_charging_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    uint32_t nchrg = GPIO_PinRead(GPIO, BOARD_INITPINS_CHRG_PORT, BOARD_INITPINS_CHRG_PIN);
    printf("\f\r\nPINT Charge Interrupt %d event detected, status = %d.", pintr, nchrg);
    if (nchrg == 1) {
    	// Charging stopped
    	GPIO_PortClear(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);
    	if (USBHSD->DEVCMDSTAT & (1ul << 28)) {
    		// VBus detected, charge finished
    		printf("VBus detected, charge finished\n");
    		cs = CS_FULLY_CHARGED;
    		// Is it possible that VBUS is present but because of poor power supply the battery is not charging?
    	}
    	else {
    		// VBus not detected, charge interrupted
    		printf("VBus not detected, charge interrupted\n");
    		cs = CS_NOT_CHARGING;
    	}
    }
    else {
        // Charging started
    	GPIO_PortSet(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);
    	if (USBHSD->DEVCMDSTAT & (1ul << 28)) {
			// VBus detected, charge started
			printf("VBus detected, charge started\n");
			cs = CS_CHARGING;
		}
		else {
			// VBus not detected, how does charge start?
			printf("VBus not detected, how is this even possible?\n");
		}
    }
}

static void hal_power_pbstat_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    bool pbstat = hal_power_is_power_button_pressed();
	printf("\f\r\nPINT Pbstat Interrupt %d event detected, status = %d.", pintr, pbstat);
	xQueueSendFromISR(g_powerButtonEventQueue, &pbstat, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

bool hal_power_is_power_button_pressed(void) {
    uint32_t pbstat = GPIO_PinRead(GPIO, BOARD_INITPINS_PB_STAT_PORT, BOARD_INITPINS_PB_STAT_PIN);
    return !pbstat;
}

// Initialize the power subsystem
void hal_power_init() {
    LTC2942_Init();

    g_powerButtonEventQueue = xQueueCreate(2, sizeof(bool));

	INPUTMUX_Init(INPUTMUX);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, PINT_CHARGE_SRC);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt1, PINT_PBSTAT_SRC);
	// Deactivate inputmux clock to save power
	INPUTMUX_Deinit(INPUTMUX);

	GPIO_PortClear(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);

    NVIC_ClearPendingIRQ(USB1_IRQn);
    NVIC_ClearPendingIRQ(USB1_NEEDCLK_IRQn);

    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY); /*< Turn on USB1 Phy */
    RESET_PeripheralReset(kUSB1H_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1D_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1RAM_RST_SHIFT_RSTn);

    CLOCK_EnableClock(kCLOCK_Usbh1);
    /* Put PHY powerdown under software control */
    *((uint32_t *)(USBHSH_BASE + 0x50)) = USBHSH_PORTMODE_SW_PDCOM_MASK;
    /* According to reference mannual, device mode setting has to be set by access usb host register */
    *((uint32_t *)(USBHSH_BASE + 0x50)) |= USBHSH_PORTMODE_DEV_ENABLE_MASK;
    /* enable usb1 host clock */
    CLOCK_DisableClock(kCLOCK_Usbh1);

	// Initialize USB device controller here so VBUS status can be read. This should be removed once USB driver is in
	CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_UsbPhySrcExt, 16000000U);
	CLOCK_EnableUsbhs0DeviceClock(kCLOCK_UsbSrcUnused, 0U);
	USB_EhciPhyInit(kUSB_ControllerLpcIp3511Hs0, 16000000U, NULL);

	// Update charging status
	hal_power_charging_callback(kPINT_PinInt0, 0);

	// Initialize PINT
	PINT_Init(PINT);
	PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableBothEdges, hal_power_charging_callback);
	PINT_PinInterruptConfig(PINT, kPINT_PinInt1, kPINT_PinIntEnableBothEdges, hal_power_pbstat_callback);
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt1);

}

void hal_power_shutdown() {
    LTC2942_Shutdown();
    GPIO_PortClear(GPIO, BOARD_INITPINS_PMIC_ON_REQ_PORT, 1u << BOARD_INITPINS_PMIC_ON_REQ_PIN);
}

// Get current voltage for reference
uint32_t hal_power_get_voltage() {
	return LTC2942_GetVoltage();
}

// Get current fuel gauge counter value
uint32_t hal_power_get_charge() {
    return LTC2942_GetCharge();
}

// Set current fuel gauge counter value
void hal_power_set_charge(uint32_t charge) {
	LTC2942_SetCharge(charge);
}

// Get stored empty level value
uint32_t hal_power_get_empty_level() {
    return LTC2942_GetLowThreshold();
}

// Set stored empty level value
void hal_power_set_empty_level(uint32_t charge) {
	LTC2942_SetLowThreshold(charge);
}

// Get stored full level value
uint32_t hal_power_get_full_level() {
    return LTC2942_GetHighThreshold();
}

// Set stored full level value
void hal_power_set_full_level(uint32_t charge) {
	LTC2942_SetHighThreshold(charge);
}

// Get charging status
ChargingStatus hal_power_get_charging_status() {
    return cs;
}
