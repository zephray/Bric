//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef _SYSTICK_H_
#define _SYSTICK_H_

void SysTick_Handler();
void SysTick_Init();
void SysTick_DelayTicks(uint32_t n);

#endif /* _SYSTICK_H_ */
