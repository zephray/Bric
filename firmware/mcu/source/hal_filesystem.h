//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef __HAL_FILESYSTEM_H__
#define __HAL_FILESYSTEM_H__

#include <stdio.h>
#include <stdint.h>
#include <dirent.h>

typedef enum {
    OM_READ,   // r
    OM_WRITE,  // w+
    OM_RW,     // r+
    OM_CREATE, // wx
    OM_APPEND  // a+
} OpenMode;

typedef enum {
    FT_REGULAR,
    FT_DIRECTORY,
    FT_OTHER
} FileType;

typedef FIL File;
typedef DIR Directory;
typedef struct {
    FileType type;
    char fname[256]; // 255 byte LFN + 1 byte
} FileInfo;

int hal_fs_init();
// Open a file, return pointer when success, NULL on error
File *hal_fs_open(char *path, OpenMode mode);
// Read from a file, return bytes read, negative on error
int hal_fs_read(File *fp, void *dst, size_t count);
// Write to a file, return bytes written, negative on error
int hal_fs_write(File *fp, void *src, size_t count);
// Get current seek, negative on error
int hal_fs_tell(File *fp);
// Get file size, negative on error
int hal_fs_size(File *fp);
// Set seek, returns the new actual seek, negative on error.
int hal_fs_seek(File *fp, size_t seek);
// Close file
int hal_fs_close(File *fp);

// Open a directory
Directory *hal_fs_opendir(char *path);
// Read directoy file
int hal_fs_readdir(Directory *dp, FileInfo* fno);
// Close a directory
int hal_fs_closedir(Directory *dp);
// Change current directory
int hal_fs_chdir(char *path);

#endif