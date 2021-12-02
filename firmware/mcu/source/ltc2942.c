//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_common.h"
#include "i2c.h"
#include "pin_mux.h"
#include "peripherals.h"
#include "ltc2942.h"

#define LTC2942_ADDR 0x64
#define LTC2942_STATUS 0x00
#define LTC2942_CTRL 0x01
#define LTC2942_CHARGE_MSB 0x02
#define LTC2942_CHARGE_LSB 0x03
#define LTC2942_CHARGE_HIGH_MSB 0x04
#define LTC2942_CHARGE_HIGH_LSB 0x05
#define LTC2942_CHARGE_LOW_MSB 0x06
#define LTC2942_CHARGE_LOW_LSB 0x07
#define LTC2942_VOLT_MSB 0x08
#define LTC2942_VOLT_LSB 0x09
#define LTC2942_TEMP_MSB 0x0c
#define LTC2942_TEMP_LSB 0x0d

/*
 * Prescaler: M >= 23 * Qbat/Ah. Given 300mAh battery, M >= 23 * 0.3 = 6.9
 * */
// ADC Sleep, Prescaler = 16, AL/CC disabled
//#define LTC2942_CTRL_VAL (0x18) // M = 8
#define LTC2942_CTRL_VAL (0x20) // M = 16

bool LTC2942_Init() {
	uint8_t regval;
	I2C_Init();

	// Shutdown mode off
	if (I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, LTC2942_CTRL_VAL) != 0) {
	    return false;
	}
/*
	I2C_ReadReg(LTC2942_ADDR, LTC2942_STATUS, &regval);
	printf("STATUS = %02x\n", regval);
	I2C_ReadReg(LTC2942_ADDR, LTC2942_CTRL, &regval);
	printf("CONTROL = %02x\n", regval);*/
	return true;
}

// in degC
uint32_t LTC2942_GetTemperature() {
	uint16_t temp;
	uint8_t regval;
	// Manual temperature mode
	I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, LTC2942_CTRL_VAL | 0x40);
	vTaskDelay(pdMS_TO_TICKS(20));
	I2C_ReadReg(LTC2942_ADDR, LTC2942_TEMP_MSB, &regval);
	temp = regval << 8;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_TEMP_LSB, &regval);
	temp |= regval & 0xff;
	uint32_t temperature = (uint32_t)temp * 600ul / 65536ul - 273ul;
	// It would automatically sleep after conversion
	//printf("Temperature: %d degC\n", temperature);

	return temperature;
}

// in mV
uint32_t LTC2942_GetVoltage() {
	uint16_t volt;
	uint8_t regval;
	// Manual voltage mode
	I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, LTC2942_CTRL_VAL | 0x80);
	vTaskDelay(pdMS_TO_TICKS(20));
	I2C_ReadReg(LTC2942_ADDR, LTC2942_VOLT_MSB, &regval);
	volt = regval << 8;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_VOLT_LSB, &regval);
	volt |= regval & 0xff;
	uint32_t voltage = (uint32_t)volt * 6000ul / 65536ul;
	//printf("Voltage: %d mV\n", voltage);

	return voltage;
}


uint32_t LTC2942_GetCharge() {
	uint16_t charge;
	uint8_t regval;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_MSB, &regval);
	charge = regval << 8;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_LSB, &regval);
	charge |= regval & 0xff;
	//printf("Charge: %04x\n", charge);
	return (uint32_t)charge & 0xffff;
}

uint32_t LTC2942_GetLowThreshold() {
    uint16_t charge;
    uint8_t regval;
    I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_LOW_MSB, &regval);
    charge = regval << 8;
    I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_LOW_LSB, &regval);
    charge |= regval & 0xff;
    return (uint32_t)charge & 0xffff;
}

uint32_t LTC2942_GetHighThreshold() {
    uint16_t charge;
    uint8_t regval;
    I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_HIGH_MSB, &regval);
    charge = regval << 8;
    I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_HIGH_LSB, &regval);
    charge |= regval & 0xff;
    return (uint32_t)charge & 0xffff;
}

void LTC2942_SetCharge(uint32_t charge) {
    LTC2942_Shutdown();
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CHARGE_MSB, (charge >> 8) & 0xff);
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CHARGE_LSB, charge & 0xff);
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, LTC2942_CTRL_VAL);
}

void LTC2942_SetLowThreshold(uint32_t charge) {
    LTC2942_Shutdown();
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CHARGE_LOW_MSB, (charge >> 8) & 0xff);
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CHARGE_LOW_LSB, charge & 0xff);
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, LTC2942_CTRL_VAL);
}

void LTC2942_SetHighThreshold(uint32_t charge) {
    LTC2942_Shutdown();
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CHARGE_HIGH_MSB, (charge >> 8) & 0xff);
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CHARGE_HIGH_LSB, charge & 0xff);
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, LTC2942_CTRL_VAL);
}

void LTC2942_Shutdown() {
    I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, LTC2942_CTRL_VAL | 0x01);
}
