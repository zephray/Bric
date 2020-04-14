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

extern SemaphoreHandle_t s_fileAccessSemaphore;

void SDC_InitSdifUnusedDataPin();
void SDC_CardDetectTask(void *pvParameters);
void SDC_Test(void *pvParameters);

#endif /* _SDCARD_H_ */
