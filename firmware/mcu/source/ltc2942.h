//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef _LTC2942_H_
#define _LTC2942_H_

bool LTC2942_Init();
uint32_t LTC2942_GetTemperature();
uint32_t LTC2942_GetVoltage();
uint32_t LTC2942_GetCharge();
void LTC2942_Shutdown();
uint32_t LTC2942_GetLowThreshold();
void LTC2942_SetCharge(uint32_t charge);
void LTC2942_SetLowThreshold(uint32_t charge);

#endif /* LTC2942_H_ */
