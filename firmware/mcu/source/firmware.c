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
#include "hal_input.h"
#include "ltc2942.h"
#include "epd.h"
#include "pm.h"
#include "app.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

#define SYSTEM_TASK_HEAPSIZE (128u)
#define SYSTEM_TASK_PRIORITY (configMAX_PRIORITIES - 1U)

void system_shutdown(void);
void system_task(void *pvParameters);

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */

	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    if (pdPASS != xTaskCreate(system_task, "System Task",
            SYSTEM_TASK_HEAPSIZE, NULL, SYSTEM_TASK_PRIORITY, NULL)) {
        // fatal
    }

	/* Start scheduling. */
	vTaskStartScheduler();

	// Should not reach here
	while(1);
}

void system_task(void *pvParameters) {
    BaseType_t result;
    bool pbstat;

    // Initialize system
    pm_init();
    hal_input_init();

    // Start application
    if (pdPASS != xTaskCreate(app_task, "App Task", APP_TASK_HEAPSIZE, NULL,
            APP_TASK_PRIORITY, NULL)) {
        // fatal
    }

    // Ignore all power key activity within 1s of startup
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (int i = 0; i < 2; i++) // Queue length = 2
        xQueueReceive(g_powerButtonEventQueue, &pbstat, pdMS_TO_TICKS(100));


    while (1) {
        result = xQueueReceive(g_powerButtonEventQueue, &pbstat,
                pdMS_TO_TICKS(50));
        if (result) {
            if (pbstat) {
                // Power button pressed, wait for 3 sec
                result = xQueueReceive(g_powerButtonEventQueue, &pbstat,
                        pdMS_TO_TICKS(3000));
                if (!result) {
                    // Not received, check again if the power key is pressed
                    pbstat = pm_is_power_button_pressed();
                    if (pbstat)
                        system_shutdown();
                }
                // otherwise, something happened, cancel shutdown
            }
        }
        hal_input_scan();

    }
}

void perf_conf_counter() {
	CTIMER1->TC = 0;
	CTIMER_StartTimer(CTIMER1);
}

uint64_t perf_get_counter() {
	return CTIMER_GetTimerCountValue(CTIMER1);
}

void vApplicationIdleHook() {
	asm("WFI");
}

void system_shutdown() {
    EPD_Init();
    EPD_Clear();
    pm_shutdown();
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
