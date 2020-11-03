//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "hal_filesystem.h"

int hal_fs_init() {
    // nothing need to be done on Linux
}

// Open a file, return pointer when success, NULL on error
File *hal_fs_open(char *path, OpenMode mode) {
    char *fopenmode;
    switch (mode)
    {
    case OM_READ:
        fopenmode = "r";
        break;
    case OM_WRITE:
        fopenmode = "w+";
        break;
    case OM_RW:
        fopenmode = "r+";
        break;
    case OM_CREATE:
        fopenmode = "wx";
        break;
    case OM_APPEND:
        fopenmode = "a+";
        break;
    default:
        fprintf(stderr, "Unsupported file open mode!\n");
        return -1;
        break;
    }
    return fopen(path, fopenmode);
}

// Read from a file, return bytes read, negative on error
int hal_fs_read(File *fp, void *dst, size_t count) {
    return fread(dst, 1, count, fp);
}

// Write to a file, return bytes written, negative on error
int hal_fs_write(File *fp, void *src, size_t count) {
    return fwrite(src, 1, count, fp);
}

// Get current seek, negative on error
int hal_fs_tell(File *fp) {
    return ftell(fp);
}

// Get file size, negative on error
int hal_fs_size(File *fp) {
    size_t seek = ftell(fp);
    fseek(fp, 0, SEEK_END);
    int size = (int)ftell(fp);
    fseek(fp, seek, SEEK_SET);
    return size;
}

// Set seek, returns the new actual seek, negative on error.
int hal_fs_seek(File *fp, size_t seek) {
    return fseek(fp, seek, SEEK_SET);
}

// Close file
int hal_fs_close(File *fp) {
    return fclose(fp);
}

// Open a directory
Directory *hal_fs_opendir(char *path) {
    return opendir(path);
}

// Read directoy file
int hal_fs_readdir(Directory *dp, FileInfo* fno) {
    struct dirent *ent = readdir(dp);
    if (ent == NULL)
        return -1;
    strcpy(fno->fname, ent->d_name);
    if (ent->d_type == DT_REG)
        fno->type = FT_REGULAR;
    else if (ent->d_type == DT_DIR)
        fno->type = FT_DIRECTORY;
    else
        fno->type = FT_OTHER;
    return 0;
}

// Close a directory
int hal_fs_closedir(Directory *dp) {
    return closedir(dp);
}

// Change current directory 
int hal_fs_chdir(char *path) {
    return chdir(path);
}