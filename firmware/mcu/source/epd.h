//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef _EPD_H_
#define _EPD_H_

void EPD_Init();
void EPD_ClearBuffer();
void EPD_Clear();
void EPD_DispImg(uint8_t *buf, bool useBackBuffer);
void EPD_SwitchToPartial();
void EPD_DispGreyscale(uint8_t* src);

#endif /* _EPD_H_ */
