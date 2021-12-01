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
#include "font.h"
#include "minimp3.h"
#include "decoder.h"
#include "batcal.h"
#include "app.h"
#include "ui.h"
#include "id3v2lib/id3v2lib.h"

bool app_get_fileext(char *fn, char *dst, int max_len) {
	int len = strlen(fn);
	int i;
	for (i = len - 1; i > 0; i--) {
		if (fn[i] == '.') break;
	}
	if (fn[i] != '.')
		return false; // dot not found
	i += 1;
	len = len - i;
	if (len > max_len) len = max_len;
	memcpy(dst, fn + i, len);
	dst[len] = '\0';
	return true;
}

int app_readdir(Directory *directory, FileInfo *fileInfo) {
	FileInfo fileInformation;
	int count = 0;
	while (1) {
		int result = hal_fs_readdir(directory, &fileInformation);
		char ext[5];

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
			fileInfo[count++] = fileInformation;
		}
		else
		{
			if (app_get_fileext(fileInformation.fname, ext, 4)) {
				if (strcmp(ext, "mp3") == 0) {
					fileInfo[count++] = fileInformation;
				}
			}
		}
	}
	return count;
}

void *app_loadfromfile(File *file, uint32_t offset, uint32_t size) {
	void *buf = pvPortMalloc(size);
	if (!buf)
		return NULL;
	hal_fs_seek(file, offset);
	hal_fs_read(file, buf, size);
	return buf;
}

void app_playfile(char *fn) {
	ui_clear();
	ui_song_info(fn);

	DecoderContext *ctx = NULL;
	ctx = pvPortMalloc(sizeof(DecoderContext));
	if (!ctx) {
		printf("Unable to allocate memory for decoder context.\r\n");
		vTaskSuspend(NULL);
	}
	dec_openfile(ctx, fn);

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
	vPortFree(ctx);
}

void app_task(void *pvParameters) {
	hal_disp_init();
	ui_init();
	ui_clear();

	hal_fs_init();

	// Check battery calibration status
	batcal_init();

	hal_audio_init();
	hal_audio_set_volume(0xa0);

	//ui_message("Warning", "Test message 测试信息\nSecond line.");
	hal_fs_chdir(APP_ROOT);

	FileInfo *fileList;

	while (1) {
		// Basic demo: list all files, let user select, playback
		// This is like 70KB of memory...
		// Up to 255 folders + files per folder, big limitation actually
		fileList = pvPortMalloc(sizeof(FileInfo) * 255);

		Directory *directory;
		directory = hal_fs_opendir(".");
		if (directory == NULL)
		{
			printf("Open directory failed.\r\n");
			vTaskSuspend(NULL);
		}

		int fileCount = app_readdir(directory, fileList);
		ui_menu_t *fileMenu = pvPortMalloc(sizeof(ui_menu_t) +
				fileCount * sizeof(ui_menu_item_t));
		strcpy(fileMenu->title, "当前目录：/");
		fileMenu->count = fileCount;
		for (int i = 0; i < fileCount; i++) {
			strncpy(fileMenu->items[i].title, fileList[i].fname, 60);
			//if (fileList[i].type == FT_DIRECTORY)
			//printf("%s\n", fileList[i].fname);
		}

		int selection = ui_run_menu(fileMenu, 0);
		FileInfo fileInformation;
		if (selection >= 0)
			fileInformation = fileList[selection];
		vPortFree(fileMenu);
		vPortFree(fileList);

		hal_fs_closedir(directory);

		if (selection >= 0) {
			if (fileList[selection].type == FT_REGULAR) {
				app_playfile(fileInformation.fname);
			}
		}
	}

	hal_audio_stop();
	vTaskSuspend(NULL);
}
