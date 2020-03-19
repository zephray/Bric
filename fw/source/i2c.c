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
#include "i2c.h"

#define I2C_CH I2C1

int I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t val) {
	if (kStatus_Success == I2C_MasterStart(I2C_CH, addr, kI2C_Write)) {
		if (kStatus_Success != I2C_MasterWriteBlocking(I2C_CH, &reg, 1, kI2C_TransferNoStopFlag)) {
			return -1;
		}

		if (kStatus_Success != I2C_MasterWriteBlocking(I2C_CH, &val, 1, kI2C_TransferDefaultFlag)) {
			return -1;
		}

		if (kStatus_Success != I2C_MasterStop(I2C_CH)) {
			return -1;
		}
	}
}

int I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t *val) {
	if (kStatus_Success == I2C_MasterStart(I2C_CH, addr, kI2C_Write)) {
		if (kStatus_Success != I2C_MasterWriteBlocking(I2C_CH, &reg, 1, kI2C_TransferNoStopFlag)) {
			return -1;
		}

		if (kStatus_Success != I2C_MasterRepeatedStart(I2C_CH, addr, kI2C_Read)) {
			return -1;
		}

		if (kStatus_Success != I2C_MasterReadBlocking(I2C_CH, &val, 1, kI2C_TransferDefaultFlag)) {
			return -1;
		}

		if (kStatus_Success != I2C_MasterStop(I2C_CH)) {
			return -1;
		}
	}
}
