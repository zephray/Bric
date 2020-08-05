//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef _CS43130_H_
#define _CS43130_H_

void CS43130_Init();
void CS43130_Reset();
void CS43130_PCMPowerUp();
void CS43130_PCMPowerDown();
void CS43130_SetVolume(uint8_t vol);
void CS43130_SwitchFreq(int freq);
void CS43130_SwtichSampleDepth(int bits);

#endif /* _CS43130_H_ */
