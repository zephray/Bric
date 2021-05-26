//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "fsl_common.h"
#include "fsl_sd_disk.h" // contains definition of g_sd
#include "fsl_iocon.h"
#include "sdcard.h"

static void SDC_DetectCallBack(bool isInserted, void *userData);

static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (100U),
    .cardInserted = SDC_DetectCallBack,
    .cardRemoved  = SDC_DetectCallBack,
};

static FATFS g_fileSystem;

static volatile bool s_cardInserted     = false;
static volatile bool s_cardInsertStatus = false;

static SemaphoreHandle_t s_CardDetectSemaphore = NULL;

/* This function is used to init the SDIF unused data pin, DATA4 - DATA7, these pin should be configured
 * ,otherswise the SDIF will not work, please check the corresponding errata.
 */
void SDC_InitSdifUnusedDataPin(void)
{
    IOCON_PinMuxSet(IOCON, 1, 27,
                    (IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[4] */
    IOCON_PinMuxSet(IOCON, 1, 28,
                    (IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[5] */
    IOCON_PinMuxSet(IOCON, 1, 29,
                    (IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[6] */
    IOCON_PinMuxSet(IOCON, 1, 30,
                    (IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[7] */
}

static void SDC_DetectCallBack(bool isInserted, void *userData)
{
    s_cardInsertStatus = isInserted;
    xSemaphoreGiveFromISR(s_CardDetectSemaphore, NULL);
}

int SDC_CardDetect(void) {
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

    s_CardDetectSemaphore = xSemaphoreCreateBinary();

    g_sd.host.base           = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd         = &s_sdCardDetect;
#if defined DEMO_SDCARD_POWER_CTRL_FUNCTION_EXIST
    g_sd.usrParam.pwr = &s_sdCardPwrCtrl;
#endif
#if defined DEMO_SDCARD_SWITCH_VOLTAGE_FUNCTION_EXIST
    g_sd.usrParam.cardVoltage = &s_sdCardVoltageSwitch;
#endif
    /* SD host init function */
    if (SD_HostInit(&g_sd) == kStatus_Success)
    {
        /* take card detect semaphore */
        if (xSemaphoreTake(s_CardDetectSemaphore, portMAX_DELAY) == pdTRUE)
        {
            if (s_cardInserted != s_cardInsertStatus)
            {
                s_cardInserted = s_cardInsertStatus;

                /* power off card */
                SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);

                if (s_cardInserted)
                {
                    printf("\r\nCard inserted.\r\n");
                    /* power on the card */
                    SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);

                    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U) != FR_OK)
                    {
                        printf("Mount volume failed.\r\n");
                        return -3;
                    }

            #if (FF_FS_RPATH >= 2U)
                    if (f_chdrive((char const *)driverNumberBuffer) != FR_OK)
                    {
                        printf("Change drive failed.\r\n");
                        return -3;
                    }
            #endif

                    return 0;
                }
            }

            if (!s_cardInserted)
            {
                printf("\r\nPlease insert a card into board.\r\n");
                return -2;
            }
        }
    }
    else
    {
        printf("\r\nSD host init fail\r\n");
        return -3;
    }

    return -1;
}

void SDC_Test(void) {
	FRESULT error;
	DIR directory; /* Directory object */
	FILINFO fileInformation;
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
    {
        printf("Mount volume failed.\r\n");
        vTaskSuspend(NULL);
    }

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)driverNumberBuffer);
    if (error)
    {
        printf("Change drive failed.\r\n");
        vTaskSuspend(NULL);
    }
#endif

    printf("\r\nList the file in that directory......\r\n");
    if (f_opendir(&directory, "/"))
    {
        printf("Open directory failed.\r\n");
        vTaskSuspend(NULL);
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

	vTaskSuspend(NULL);
}
