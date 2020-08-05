//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef _I2C_H_
#define _I2C_H_

void I2C_Init();
int I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t val);
int I2C_WritePayload(uint8_t addr, uint8_t *payload, size_t len);
int I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t *val);
int I2C_ReadPayload(uint8_t addr, uint8_t *tx_payload, size_t tx_len, uint8_t *rx_payload, size_t rx_len);

#endif
