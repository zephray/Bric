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

int I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t val);
int I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t *val);

#endif
