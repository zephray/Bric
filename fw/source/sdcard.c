//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include "fsl_common.h"
#include "fsl_sd_disk.h" // contains definition of g_sd
#include "sdcard.h"

static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};

static FATFS g_fileSystem;
static FIL g_fileObject;

int SDC_CheckCardInsert() {
	/* Save host information. */
	g_sd.host.base = SD_HOST_BASEADDR;
	g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
	/* card detect type */
	g_sd.usrParam.cd = &s_sdCardDetect;

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        printf("\r\nSD host init fail\r\n");
        return -1;
    }
    /* power off card */
    SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
    /* wait card insert */
    if (SD_WaitCardDetectStatus(SD_HOST_BASEADDR, &s_sdCardDetect, true) == kStatus_Success)
    {
    	printf("\r\nCard inserted.\r\n");
        /* power on the card */
        SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
    }
    else
    {
    	printf("\r\nCard detect fail.\r\n");
        return -1;
    }

    return 0;
}

int SDC_Init() {
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

	if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
	{
		PRINTF("Mount volume failed.\r\n");
		return -1;
	}

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)driverNumberBuffer);
    if (error)
    {
        PRINTF("Change drive failed.\r\n");
        return -1;
    }
#endif
}
