//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "ff.h"
#include "sdcard.h"
#include "hal_filesystem.h"

static SemaphoreHandle_t s_fileAccessSemaphore = NULL;

int hal_fs_init() {
    s_fileAccessSemaphore = xSemaphoreCreateBinary();

    int result = SDC_CardDetect();

    xSemaphoreGive(s_fileAccessSemaphore);

    return 0;
}

// Open a file, return pointer when success, NULL on error
File *hal_fs_open(char *path, OpenMode mode) {
    BYTE fatfsmode;
    File *file = pvPortMalloc(sizeof(File));
    FRESULT result;
    switch (mode)
    {
    case OM_READ:
        fatfsmode = FA_READ;
        break;
    case OM_WRITE:
        fatfsmode = FA_CREATE_ALWAYS | FA_READ | FA_WRITE;
        break;
    case OM_RW:
        fatfsmode = FA_READ | FA_WRITE;
        break;
    case OM_CREATE:
        fatfsmode = FA_CREATE_NEW | FA_READ | FA_WRITE;
        break;
    case OM_APPEND:
        fatfsmode = FA_OPEN_APPEND | FA_READ | FA_WRITE;
        break;
    default:
        printf("Unsupported file open mode!\n");
        return NULL;
        break;
    }
    if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
        printf("Unable to obtain lock to access filesystem.\n");
    }
    result = f_open((FIL *)file, path, fatfsmode);
    xSemaphoreGive(s_fileAccessSemaphore);
    if (result != FR_OK) {
        vPortFree(file);
        return NULL;
    }
    else {
        return file;
    }
}

// Read from a file, return bytes read, negative on error
int hal_fs_read(File *fp, void *dst, size_t count) {
    uint32_t bytes;
    if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
        printf("Unable to obtain lock to access filesystem.\n");
    }
    FRESULT result = f_read(fp, dst, count, &bytes);
    xSemaphoreGive(s_fileAccessSemaphore);
    if (result != FR_OK)
        return -1;
    else
        return bytes;
}

// Write to a file, return bytes written, negative on error
int hal_fs_write(File *fp, void *src, size_t count) {
    uint32_t bytes;
    if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
        printf("Unable to obtain lock to access filesystem.\n");
    }
    FRESULT result = f_write(fp, src, count, &bytes);
    xSemaphoreGive(s_fileAccessSemaphore);
    if (result != FR_OK)
        return -1;
    else
        return bytes;
}

// Get current seek, negative on error
int hal_fs_tell(File *fp) {
    return f_tell(fp);
}

// Get file size, negative on error
int hal_fs_size(File *fp) {
    return f_size(fp);
}

// Set seek, returns the new actual seek, negative on error.
int hal_fs_seek(File *fp, size_t seek) {
    if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
        printf("Unable to obtain lock to access filesystem.\n");
    }
    FRESULT result = f_lseek(fp, seek);
    xSemaphoreGive(s_fileAccessSemaphore);
    return (result == FR_OK) ? 0 : -1;
}

// Close file
int hal_fs_close(File *fp) {
    FRESULT result = f_close(fp);
    return (result == FR_OK) ? 0 : -1;
}

// Open a directory
Directory *hal_fs_opendir(char *path) {
    Directory *dir = malloc(sizeof(Directory));
    if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
        printf("Unable to obtain lock to access filesystem.\n");
    }
    FRESULT result = f_opendir(dir, path);
    xSemaphoreGive(s_fileAccessSemaphore);
    if (result == FR_OK) {
        return dir;
    }
    else {
        free(dir);
        return NULL;
    }
}

// Read directoy file
int hal_fs_readdir(Directory *dp, FileInfo* fno) {
    FILINFO fatfs_fno;
    if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
        printf("Unable to obtain lock to access filesystem.\n");
    }
    FRESULT result = f_readdir(dp, &fatfs_fno);
    xSemaphoreGive(s_fileAccessSemaphore);
    if (result != FR_OK) {
        return -1;
    }
    strcpy(fno->fname, fatfs_fno.fname);
    // FT_OTHER is not used for FAT
    if (fatfs_fno.fattrib &= AM_DIR)
        fno->type = FT_DIRECTORY;
    else
        fno->type = FT_REGULAR;
    return 0;
}

// Close a directory
int hal_fs_closedir(Directory *dp) {
    if (xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) != pdTRUE) {
        printf("Unable to obtain lock to access filesystem.\n");
    }
    FRESULT result = f_closedir(dp);
    xSemaphoreGive(s_fileAccessSemaphore);
    return (result == FR_OK) ? 0 : -1;
}

// Change current directory
int hal_fs_chdir(char *path) {
    FRESULT result = f_chdir(path);
    return (result == FR_OK) ? 0 : -1;
}
