//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "fsl_common.h"
#include "systick.h"
#include "fsl_sdmmc_event.h"

//static volatile size_t systick_counter;

/*void SysTick_Handler() {
    if (systick_counter != 0U) {
        systick_counter--;
    }
}*/

void SysTick_Init() {
	/* Set systick reload value to generate 1ms interrupt */
	//if (SysTick_Config(SystemCoreClock / 1000U)) {
	//	while (1);
	//}
}

void SysTick_DelayTicks(uint32_t n) {
	/* Set the counter and wait till it reaches zero */
	//systick_counter = n;
    //while (systick_counter != 0U);
	SDMMCEVENT_Delay(n);
}
