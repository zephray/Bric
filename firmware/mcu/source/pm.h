//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef _PM_H_
#define _PM_H_

extern QueueHandle_t g_powerButtonEventQueue;

void pm_init();
void pm_shutdown();
bool pm_is_power_button_pressed(void);

#endif /* PM_H_ */
