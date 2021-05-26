//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef _SDCARD_H_
#define _SDCARD_H_

void SDC_InitSdifUnusedDataPin();
int SDC_CardDetect(void);
void SDC_Test(void);

#endif /* _SDCARD_H_ */
