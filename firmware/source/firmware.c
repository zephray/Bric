/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    firmware.c
 * @brief   Application entry point.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include <stdio.h>
#include "ff.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC5528.h"
#include "fsl_ostimer.h"
#include "fsl_ctimer.h"
#include "sdcard.h"
#include "hal_display.h"
#include "hal_audio.h"
#include "minimp3.h"
#include "decoder.h"
#include "font.h"
#include "ltc2942.h"
#include "pm.h"
#include "tags.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

static void ui_task(void *pvParameters);

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */

	BOARD_InitBootPins();
    //SDC_InitSdifUnusedDataPin();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    /*if (pdPASS != xTaskCreate(SDC_Test, "SDCTestTask", 1024U, NULL,	(configMAX_PRIORITIES - 2U), NULL))
	{
		return -1;
	}*/

	if (pdPASS !=
		xTaskCreate(SDC_CardDetectTask, "SDC Init", 512U, NULL, (configMAX_PRIORITIES - 1U), NULL))
	{
		return -1;
	}

	if (pdPASS !=
		xTaskCreate(ui_task, "UI Task", 1024U, NULL, (configMAX_PRIORITIES - 2U), NULL))
	{
		return -1;
	}


	/* Start scheduling. */
	vTaskStartScheduler();

	// Should not reach here
	while(1);
}

/*volatile uint32_t perf_counter = 0;

void ctimer_match0_callback(uint32_t flags)
{
    perf_counter++;
}

ctimer_callback_t ctimer_callback_table[] = {
    NULL, ctimer_match0_callback, NULL, NULL, NULL, NULL, NULL, NULL};*/

void perf_conf_counter() {
	//OSTIMER_Init(OSTIMER);
	/*static ctimer_match_config_t matchConfig0;
	matchConfig0.enableCounterReset = true;
	matchConfig0.enableCounterStop  = false;
	matchConfig0.matchValue         = CLOCK_GetCTimerClkFreq(1U) / 2;
	matchConfig0.outControl         = kCTIMER_Output_NoAction;
	matchConfig0.outPinInitState    = false;
	matchConfig0.enableInterrupt    = true;
	CTIMER_RegisterCallBack(CTIMER1, &ctimer_callback_table[0], kCTIMER_MultipleCallback);
	CTIMER_SetupMatch(CTIMER1, kCTIMER_Match_1, &matchConfig0);
	perf_counter = 0;*/
	CTIMER1->TC = 0;
	CTIMER_StartTimer(CTIMER1);
}

uint64_t perf_get_counter() {
	//return OSTIMER_GetCurrentTimerValue(OSTIMER);
	//return perf_counter;
	return CTIMER_GetTimerCountValue(CTIMER1);
}

void vApplicationIdleHook() {
	asm("WFI");
}

/* This example demonstrates how a human readable table of run time stats
information is generated from raw data provided by uxTaskGetSystemState().
The human readable table is written to pcWriteBuffer.  (see the vTaskList()
API function which actually does just this). */
void generatePerfReport()
{
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	uint32_t ulTotalRunTime, ulStatsAsPercentage;

   /* Take a snapshot of the number of tasks in case it changes while this
   function is executing. */
   uxArraySize = uxTaskGetNumberOfTasks();

   /* Allocate a TaskStatus_t structure for each task.  An array could be
   allocated statically at compile time. */
   pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

   if( pxTaskStatusArray != NULL )
   {
      /* Generate raw status information about each task. */
      uxArraySize = uxTaskGetSystemState( pxTaskStatusArray,
                                 uxArraySize,
                                 &ulTotalRunTime );

      /* For percentage calculations. */
      ulTotalRunTime /= 100UL;
      printf("Task\t\tTime\t\tPercent\n");
      /* Avoid divide by zero errors. */
      if( ulTotalRunTime > 0 )
      {
         /* For each populated position in the pxTaskStatusArray array,
         format the raw data as human readable ASCII data. */
         for( x = 0; x < uxArraySize; x++ )
         {
            /* What percentage of the total run time has the task used?
            This will always be rounded down to the nearest integer.
            ulTotalRunTimeDiv100 has already been divided by 100. */
            ulStatsAsPercentage =
                  pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

            if( ulStatsAsPercentage > 0UL )
            {
               printf("%s\t\t%lu\t\t%lu\%\n",
                                 pxTaskStatusArray[ x ].pcTaskName,
                                 pxTaskStatusArray[ x ].ulRunTimeCounter,
                                 ulStatsAsPercentage );
            }
            else
            {
               /* If the percentage is zero here then the task has
               consumed less than 1% of the total run time. */
               printf("%s\t\t%lu\t\t<1\%\n",
                                 pxTaskStatusArray[ x ].pcTaskName,
                                 pxTaskStatusArray[ x ].ulRunTimeCounter );
            }

         }
      }

      /* The array is no longer needed, free the memory it consumes. */
      vPortFree( pxTaskStatusArray );
   }
}


static void ui_task(void *pvParameters) {
	LTC2942_Init();
	LTC2942_GetVoltage();
	LTC2942_GetTemperature();
	LTC2942_GetCharge();

	pm_init();

	hal_disp_init();

	Canvas *fb = hal_disp_create(256, 128, PIXFMT_Y1);
	hal_disp_fill(fb, 0, 0, 256, 128, 1);

	font_init();
	//font_disp(fb, 0, 0, 0, "Hello World", 12, CE_ASCII);

	FRESULT error;
	DIR directory; /* Directory object */
	FILINFO fileInformation;
	int y = 0;

	printf("\r\nList the file in that directory......\r\n");
	if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
		printf("Unable to obtain lock to access filesystem.\n");
	}
	if (f_opendir(&directory, "/"))
	{
		printf("Open directory failed.\r\n");
		vTaskSuspend(NULL);
	}
	xSemaphoreGive(s_fileAccessSemaphore);

	for (;;)
	{
		if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
			printf("Unable to obtain lock to access filesystem.\n");
		}
		error = f_readdir(&directory, &fileInformation);
		xSemaphoreGive(s_fileAccessSemaphore);

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
			if (fileInformation.fname[0] == 'o')
				continue;
			if (fileInformation.fname[0] == 'O')
				continue;
			//font_disp(fb, 0, y, 0, fileInformation.fname, 64, CE_UTF8);
			y += 16;
			break; // Play the first file for now
		}
	}
	//hal_disp_draw(fb, REFRESH_PARTIAL);

	Canvas *cover = hal_disp_create(256, 128, PIXFMT_Y8);
	hal_disp_fill(fb, 0, 0, 250, 128, 0xff);

	tags_open(fileInformation.fname);
	tags_display_cover(cover->buf, 250, 122, 122);

	hal_disp_draw(cover, REFRESH_FULL);
	hal_disp_free(cover);

	char *title = tags_get_title();
	if (title) {
		if ((title[0] == 0x01) || (title[0] == 0x02)) {
			printf("Title is encoded in UTF16\n");
			font_disp(fb, 130, 32, 0, title + 1, 64, CE_UTF16);
		}
		else {
			printf("Title is encoded in UTF8\n");
			font_disp(fb, 130, 32, 0, title + 1, 64, CE_UTF8);
		}
	}
	vPortFree(title);
	char *album = tags_get_album();
	font_disp(fb, 130, 48, 0, album + 1, 64, ((album[0] == 0x01) || (album[0] == 0x02)) ? CE_UTF16 : CE_UTF8);
	vPortFree(album);

	char *artist = tags_get_artist();
	font_disp(fb, 130, 64, 0, artist + 1, 64, ((artist[0] == 0x01) || (artist[0] == 0x02)) ? CE_UTF16 : CE_UTF8);
	vPortFree(artist);

	tags_close();

	hal_disp_draw(fb, REFRESH_PARTIAL);

	hal_audio_init();
	hal_audio_set_volume(0xa0);

	DecoderContext *ctx = NULL;
	ctx = pvPortMalloc(sizeof(DecoderContext));
	if (!ctx) {
		printf("Unable to allocate memory for decoder context.\r\n");
		vTaskSuspend(NULL);
	}
	dec_openfile(ctx, fileInformation.fname);

	uint32_t charge = LTC2942_GetCharge();
	uint32_t tick = perf_get_counter();

	// Playback
	hal_audio_start(44100, AF_S16LE, dec_audio_callback, ctx);
	dec_play(ctx);

	while (!ctx->finished) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	charge -= LTC2942_GetCharge();
	tick = perf_get_counter() - tick;
	uint32_t voltage = LTC2942_GetVoltage();

	generatePerfReport();

	dec_close(ctx);
	hal_audio_stop();

	vPortFree(ctx);

	/*uint32_t power = (uint32_t)(((float)charge * 0.085f) / ((float) tick / 10000.0f / 3600.0f) * ((float)voltage / 1000.0f));
	char *buf = pvPortMalloc(128);
	sprintf(buf, "Coulomb Counter: %d LSB", charge);
	font_disp(fb, 0, y, 0, buf, 64, CE_UTF8);
	y+=16;
	sprintf(buf, "Voltage: %d mV", voltage);
	font_disp(fb, 0, y, 0, buf, 64, CE_UTF8);
	y+=16;
	sprintf(buf, "Time: %d ms", tick / 10);
	font_disp(fb, 0, y, 0, buf, 64, CE_UTF8);
	y+=16;
	sprintf(buf, "Power: %d mW", power);
	font_disp(fb, 0, y, 0, buf, 64, CE_UTF8);
	y+=16;
	hal_disp_draw(fb, REFRESH_PARTIAL);*/

 	vTaskSuspend(NULL);
}
