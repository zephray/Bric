//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef __APP_H__
#define __APP_H__

#define STARTUP_TASK_HEAPSIZE (1024u)

#define STARTUP_TASK_PRIORITY (configMAX_PRIORITIES - 2U)

void startup_task(void *pvParameters);

#endif
