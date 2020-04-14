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

static void wav_header(uint8_t *header, uint32_t sampleRate, uint32_t bitsPerFrame, uint32_t fileSize)
{
    uint32_t totalDataLen = fileSize - 8U;
    uint32_t audioDataLen = fileSize - 44U;
    uint32_t byteRate = sampleRate * (bitsPerFrame / 8U) * 2U;
    header[0] = 'R';
    header[1] = 'I';
    header[2] = 'F';
    header[3] = 'F';
    header[4] = (totalDataLen & 0xff); /* file-size (equals file-size - 8) */
    header[5] = ((totalDataLen >> 8U) & 0xff);
    header[6] = ((totalDataLen >> 16U) & 0xff);
    header[7] = ((totalDataLen >> 24U) & 0xff);
    header[8] = 'W'; /* Mark it as type "WAVE" */
    header[9] = 'A';
    header[10] = 'V';
    header[11] = 'E';
    header[12] = 'f'; /* Mark the format section 'fmt ' chunk */
    header[13] = 'm';
    header[14] = 't';
    header[15] = ' ';
    header[16] = 16; /* 4 bytes: size of 'fmt ' chunk, Length of format data.  Always 16 */
    header[17] = 0;
    header[18] = 0;
    header[19] = 0;
    header[20] = 1; /* format = 1 ,Wave type PCM */
    header[21] = 0;
    header[22] = 2; /* channels */
    header[23] = 0;
    header[24] = (sampleRate & 0xff);
    header[25] = ((sampleRate >> 8U) & 0xff);
    header[26] = ((sampleRate >> 16U) & 0xff);
    header[27] = ((sampleRate >> 24U) & 0xff);
    header[28] = (byteRate & 0xff);
    header[29] = ((byteRate >> 8U) & 0xff);
    header[30] = ((byteRate >> 16U) & 0xff);
    header[31] = ((byteRate >> 24U) & 0xff);
    header[32] = (2 * bitsPerFrame / 8); /* block align */
    header[33] = 0;
    header[34] = 16; /* bits per sample */
    header[35] = 0;
    header[36] = 'd'; /*"data" marker */
    header[37] = 'a';
    header[38] = 't';
    header[39] = 'a';
    header[40] = (audioDataLen & 0xff); /* data-size (equals file-size - 44).*/
    header[41] = ((audioDataLen >> 8) & 0xff);
    header[42] = ((audioDataLen >> 16) & 0xff);
    header[43] = ((audioDataLen >> 24) & 0xff);
}

/* This example demonstrates how a human readable table of run time stats
information is generated from raw data provided by uxTaskGetSystemState().
The human readable table is written to pcWriteBuffer.  (see the vTaskList()
API function which actually does just this). */
void generatePerfReport()
{
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	unsigned long ulTotalRunTime, ulStatsAsPercentage;

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
			font_disp(fb, 0, y, 0, fileInformation.fname, 64, CE_UTF8);
			y += 16;
			break; // Play the first file for now
		}
	}

	/*hal_disp_set(fb, 1, 1, 0);
	hal_disp_set(fb, 1, 10, 0);
	hal_disp_set(fb, 10, 1, 0);*/
	hal_disp_draw(fb, REFRESH_PARTIAL);

	hal_audio_init();

	DecoderContext *ctx = NULL;
	ctx = pvPortMalloc(sizeof(DecoderContext));
	if (!ctx) {
		printf("Unable to allocate memory for decoder context.\r\n");
		vTaskSuspend(NULL);
	}
	dec_openfile(ctx, fileInformation.fname);

	// Playback
	hal_audio_start(44100, AF_S16LE, dec_audio_callback, ctx);
	dec_play(ctx);

	while (!ctx->finished) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	// Convert to WAV
	/*dec_play(ctx);

	FIL outputFile;
	uint8_t *outputBuf = pvPortMalloc(MAX_FRAME_SIZE);
	if (!outputBuf) {
		printf("Unable to create output buffer!\n");
		vTaskSuspend(NULL);
	}

	if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
		printf("Unable to obtain lock to access filesystem.\n");
		vTaskSuspend(NULL);
	}
	error = f_open(&outputFile, "out.wav", FA_CREATE_ALWAYS | FA_WRITE);
	if (error != FR_OK) {
		printf("Unable to create output file!\n");
		vTaskSuspend(NULL);
	}
	error = f_lseek(&outputFile, 44);
	if (error != FR_OK) {
		printf("Unable to seek to designated output place!\n");
		vTaskSuspend(NULL);
	}
	xSemaphoreGive(s_fileAccessSemaphore);

	uint32_t bytes;
	while (!ctx->finished) {
		uint32_t size = dec_audio_callback(ctx, outputBuf, MAX_FRAME_SIZE);
		if (size != 0) {
			xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY);
			error = f_write(&outputFile, outputBuf, size, &bytes);
			xSemaphoreGive(s_fileAccessSemaphore);
			if ((error != FR_OK) || (bytes != size)) {
				printf("Unable to write to output file!\n");
				vTaskSuspend(NULL);
			}
		}
		else {
			printf("Didn't get any data in the last frame!\n");
		}
		// Each MP3 frame lasts 26 ms
		vTaskDelay(pdMS_TO_TICKS(26));
	}

	uint32_t filesize = f_tell(&outputFile);
	wav_header(outputBuf, 44100, 16, filesize);
	xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY);
	f_lseek(&outputFile, 0);
	f_write(&outputFile, outputBuf, 44, &bytes);
	f_close(&outputFile);
	xSemaphoreGive(s_fileAccessSemaphore);

	vPortFree(outputBuf);
	*/

	generatePerfReport();

	dec_close(ctx);

	vPortFree(ctx);

	vTaskSuspend(NULL);
}
