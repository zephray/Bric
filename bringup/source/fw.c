/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    fw.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_power.h"
#include "fsl_clock.h"
#include "LPC5528.h"
#include "fsl_gpio.h"
#include "systick.h"
#include "i2c.h"
#include "epd.h"
#include "cs43130.h"
#include "sdcard.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    //BOARD_BootClockFROHF96M();
    BOARD_InitBootPeripherals();

    GPIO_PortSet(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);

    printf("Project Bric Bringup\n");

    //printf("Initializing SysTick...\n");
    //SysTick_Init();

    /* SD Card */
	if (SDC_CheckCardInsert() == 0) {
		//SDC_Init();
	}

    /* Gas Gauge */
    printf("Testing LTC2942 Gas Gauge...\n");
    //LTC2942 address 1100100
#define LTC2942_ADDR 0x64
#define LTC2942_STATUS 0x00
#define LTC2942_CTRL 0x01
#define LTC2942_VOLT_MSB 0x08
#define LTC2942_VOLT_LSB 0x09
#define LTC2942_TEMP_MSB 0x0c
#define LTC2942_TEMP_LSB 0x0d
    uint8_t regval;
    I2C_ReadReg(LTC2942_ADDR, LTC2942_STATUS, &regval);
    printf("STATUS = %02x\n", regval);
    I2C_ReadReg(LTC2942_ADDR, LTC2942_CTRL, &regval);
    printf("CONTROL = %02x\n", regval);

    // Start voltage measure
    uint16_t volt;
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, 0xBC);
    SysTick_DelayTicks(10U);
    I2C_ReadReg(LTC2942_ADDR, LTC2942_VOLT_MSB, &regval);
    volt = regval << 8;
    I2C_ReadReg(LTC2942_ADDR, LTC2942_VOLT_LSB, &regval);
    volt |= regval & 0xff;
    uint32_t voltage = (uint32_t)volt * 6000ul / 65536ul;
    printf("Voltage: %d mV\n", voltage);

    // Start temperature measure
	uint16_t temp;
	I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, 0x7C);
	SysTick_DelayTicks(10U);
	I2C_ReadReg(LTC2942_ADDR, LTC2942_TEMP_MSB, &regval);
	temp = regval << 8;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_TEMP_LSB, &regval);
	temp |= regval & 0xff;
	uint32_t temperature = (uint32_t)temp * 600ul / 65536ul - 273ul;
	printf("Temperature: %d degC\n", temperature);

    /* SPI Flash */
    /* Not part of the prototype yet */

    /* Eink */
    //EPD_Init();
    //EPD_Clear();
    //EPD_SwitchToGreyscale();
    //EPD_Test();
    //EPD_Test2();
    //EPD_DeepSleep();

    /* Codec */
	//printf("Initializing audio codec...\n");
	//CS43130_Reset();
    //CS43130_Init();

    //printf("Initializing audio dma...\n");
    //AUDIO_Init();
    //AUDIO_Start();

    /* USB Audio */
	//USB_AudioSpeakerInit();

	GPIO_PortClear(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);

    while(1) {
    	//GPIO_PortToggle(GPIO, BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_PIN);
    	//SysTick_DelayTicks(500U);
    	//USB_AudioSpeakerLoop();
    }
    return 0 ;
}
