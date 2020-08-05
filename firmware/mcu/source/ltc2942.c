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
#define LTC2942_VOLT_MSB 0x08
#define LTC2942_VOLT_LSB 0x09
#define LTC2942_TEMP_MSB 0x0c
#define LTC2942_TEMP_LSB 0x0d

void LTC2942_Init() {
	uint8_t regval;
	I2C_Init();

	I2C_ReadReg(LTC2942_ADDR, LTC2942_STATUS, &regval);
	printf("STATUS = %02x\n", regval);
	I2C_ReadReg(LTC2942_ADDR, LTC2942_CTRL, &regval);
	printf("CONTROL = %02x\n", regval);
}

// in degC
uint32_t LTC2942_GetTemperature() {
	uint16_t temp;
	uint8_t regval;
	I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, 0x7C);
	vTaskDelay(pdMS_TO_TICKS(20));
	I2C_ReadReg(LTC2942_ADDR, LTC2942_TEMP_MSB, &regval);
	temp = regval << 8;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_TEMP_LSB, &regval);
	temp |= regval & 0xff;
	uint32_t temperature = (uint32_t)temp * 600ul / 65536ul - 273ul;
	printf("Temperature: %d degC\n", temperature);

	return temperature;
}

// in mV
uint32_t LTC2942_GetVoltage() {
	uint16_t volt;
	uint8_t regval;
	I2C_WriteReg(LTC2942_ADDR, LTC2942_CTRL, 0xBC);
	vTaskDelay(pdMS_TO_TICKS(20));
	I2C_ReadReg(LTC2942_ADDR, LTC2942_VOLT_MSB, &regval);
	volt = regval << 8;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_VOLT_LSB, &regval);
	volt |= regval & 0xff;
	uint32_t voltage = (uint32_t)volt * 6000ul / 65536ul;
	printf("Voltage: %d mV\n", voltage);

	return voltage;
}


uint32_t LTC2942_GetCharge() {
	uint16_t charge;
	uint8_t regval;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_MSB, &regval);
	charge = regval << 8;
	I2C_ReadReg(LTC2942_ADDR, LTC2942_CHARGE_LSB, &regval);
	charge |= regval & 0xff;
	printf("Charge: %04x\n", charge);
	return (uint32_t)charge & 0xffff;
}
