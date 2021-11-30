//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include <stdio.h>
#include <stdbool.h>
#include "platform.h"
#include "hal_filesystem.h"
#include "hal_display.h"
#include "hal_audio.h"
#include "hal_power.h"
#include "minimp3.h"
#include "decoder.h"
#include "batcal.h"
#include "app.h"
#include "ui.h"

void app_task(void *pvParameters) {
	hal_disp_init();
	ui_init();
	ui_clear();
	// Check battery calibration status
	batcal_init();

	ui_message("Warning", "Test message 测试信息\nSecond line.");

	/*hal_fs_chdir(APP_ROOT);

	Directory *directory;
	FileInfo fileInformation;
	int y = 0;

	printf("\r\nList the file in that directory......\r\n");
	directory = hal_fs_opendir(".");
	if (directory == NULL)
	{
		printf("Open directory failed.\r\n");
		vTaskSuspend(NULL);
	}
	for (;;)
	{
		int result = hal_fs_readdir(directory, &fileInformation);

		if ((result < 0) || (fileInformation.fname[0U] == 0U))
		{
		   break;
		}
		if (fileInformation.fname[0] == '.')
		{
		   continue;
		}
		if (fileInformation.type == FT_DIRECTORY)
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
			//break; // Play the first file for now
		}
	}
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

	//uint32_t charge = LTC2942_GetCharge();
	//uint32_t tick = perf_get_counter();

	// Playback
	if (hal_audio_start(44100, AF_S16LE, dec_audio_callback, ctx) < 0) {
		printf("Unable to start audio subsystem.\r\n");
		vTaskSuspend(NULL);
	}
	dec_play(ctx);

	while (!ctx->finished) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	//charge -= LTC2942_GetCharge();
	//tick = perf_get_counter() - tick;
	//uint32_t voltage = LTC2942_GetVoltage();

	//generatePerfReport();

	dec_close(ctx);
	hal_audio_stop();

	vPortFree(ctx);*/

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
