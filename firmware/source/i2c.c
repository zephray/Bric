//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "fsl_i2c.h"
#include "peripherals.h"
#include "i2c.h"
#include "fsl_i2c_freertos.h"

#define I2C_CH I2C1
#define I2C_MASTER ((I2C_Type *)I2C1_BASE)
#define I2C_BAUDRATE (100000) /* 100K */
#define I2C_MASTER_CLOCK_FREQUENCY (FLEXCOMM1_CLOCK_SOURCE)

i2c_rtos_handle_t i2c_rtos_handle;
static bool initialized;

void I2C_Init() {
	i2c_master_config_t masterConfig;
	status_t status;

	if (initialized)
		return;

	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = I2C_BAUDRATE;

	status = I2C_RTOS_Init(&i2c_rtos_handle, I2C_MASTER, &masterConfig, I2C_MASTER_CLOCK_FREQUENCY);
	if (status != kStatus_Success) {
		printf("I2C master: error during init, %d", status);
	}

	initialized = true;
}

int I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t val) {
	i2c_master_transfer_t masterXfer;
	status_t status;
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = val;

	masterXfer.slaveAddress   = addr;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = buf;
	masterXfer.dataSize       = 2;
	masterXfer.flags          = kI2C_TransferDefaultFlag;

    status = I2C_RTOS_Transfer(&i2c_rtos_handle, &masterXfer);
    if (status != kStatus_Success) {
    	printf("I2C master: error during write transaction, %d", status);
        return -1;
    }

	return 0;
}

int I2C_WritePayload(uint8_t addr, uint8_t *payload, size_t len) {
	i2c_master_transfer_t masterXfer;
	status_t status;

	masterXfer.slaveAddress   = addr;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = payload;
	masterXfer.dataSize       = len;
	masterXfer.flags          = kI2C_TransferDefaultFlag;

    status = I2C_RTOS_Transfer(&i2c_rtos_handle, &masterXfer);
    if (status != kStatus_Success) {
    	printf("I2C master: error during write transaction, %d", status);
        return -1;
    }

	return 0;
}

int I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t *val) {
	i2c_master_transfer_t masterXfer;
	status_t status;

	masterXfer.slaveAddress   = addr;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = &reg;
	masterXfer.dataSize       = 1;
	masterXfer.flags          = kI2C_TransferNoStopFlag;

    status = I2C_RTOS_Transfer(&i2c_rtos_handle, &masterXfer);
    if (status != kStatus_Success) {
    	printf("I2C master: error during write transaction, %d", status);
        return -1;
    }

	masterXfer.direction      = kI2C_Read;
	masterXfer.data           = val;
	masterXfer.flags          = kI2C_TransferDefaultFlag;

    status = I2C_RTOS_Transfer(&i2c_rtos_handle, &masterXfer);
    if (status != kStatus_Success) {
    	printf("I2C master: error during read transaction, %d", status);
        return -1;
    }

	return 0;
}

int I2C_ReadPayload(uint8_t addr, uint8_t *tx_payload, size_t tx_len, uint8_t *rx_payload, size_t rx_len) {
	i2c_master_transfer_t masterXfer;
	status_t status;

	masterXfer.slaveAddress   = addr;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = tx_payload;
	masterXfer.dataSize       = tx_len;
	masterXfer.flags          = kI2C_TransferNoStopFlag;

    status = I2C_RTOS_Transfer(&i2c_rtos_handle, &masterXfer);
    if (status != kStatus_Success) {
    	printf("I2C master: error during write transaction, %d", status);
        return -1;
    }

	masterXfer.direction      = kI2C_Read;
	masterXfer.data           = rx_payload;
	masterXfer.dataSize       = rx_len;
	masterXfer.flags          = kI2C_TransferDefaultFlag;

    status = I2C_RTOS_Transfer(&i2c_rtos_handle, &masterXfer);
    if (status != kStatus_Success) {
    	printf("I2C master: error during read transaction, %d", status);
        return -1;
    }

	return 0;
}
