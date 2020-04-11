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
    .cdTimeOut_ms = (100U),
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
    if (SD_WaitCardDetectStatus(SD_HOST_BASEADDR, &s_sdCardDetect, false) == kStatus_Success)
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
	FRESULT error;
	DIR directory; /* Directory object */
	FILINFO fileInformation;
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

	if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
	{
		printf("Mount volume failed.\r\n");
		return -1;
	}

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)driverNumberBuffer);
    if (error)
    {
        printf("Change drive failed.\r\n");
        return -1;
    }
#endif

	printf("\r\nList the file in that directory......\r\n");
	if (f_opendir(&directory, "/"))
	{
		printf("Open directory failed.\r\n");
	   return -1;
	}

	for (;;)
	{
		error = f_readdir(&directory, &fileInformation);

		/* To the end. */
		if ((error != FR_OK) || (fileInformation.fname[0U] == 0U))
		{
		   break;
		}
		if (fileInformation.fname[0] == '.')
		{
		   continue;
		}
		if (fileInformation.fattrib & AM_DIR)
		{
			printf("Directory file : %s.\r\n", fileInformation.fname);
		}
		else
		{
			printf("General file : %s.\r\n", fileInformation.fname);
		}
	}

    return 0;
}
