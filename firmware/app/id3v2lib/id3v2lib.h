/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef id3v2lib_id3v2lib_h
#define id3v2lib_id3v2lib_h

#ifdef __cplusplus
extern "C" {
#endif

#include "id3v2lib/types.h"
#include "id3v2lib/constants.h"
#include "id3v2lib/header.h"
#include "id3v2lib/frame.h"
#include "id3v2lib/utils.h"

ID3v2_tag* load_tag(const char* file_name);
ID3v2_tag* load_tag_with_file(File *file);
void remove_tag(const char* file_name);
void set_tag(const char* file_name, ID3v2_tag* tag);

// Getter functions
ID3v2_frame* tag_get_title(ID3v2_tag* tag);
ID3v2_frame* tag_get_artist(ID3v2_tag* tag);
ID3v2_frame* tag_get_album(ID3v2_tag* tag);
ID3v2_frame* tag_get_album_artist(ID3v2_tag* tag);
ID3v2_frame* tag_get_genre(ID3v2_tag* tag);
ID3v2_frame* tag_get_track(ID3v2_tag* tag);
ID3v2_frame* tag_get_year(ID3v2_tag* tag);
ID3v2_frame* tag_get_comment(ID3v2_tag* tag);
ID3v2_frame* tag_get_disc_number(ID3v2_tag* tag);
ID3v2_frame* tag_get_composer(ID3v2_tag* tag);
ID3v2_frame* tag_get_album_cover(ID3v2_tag* tag);

#ifdef __cplusplus
} // end of extern C
#endif

#endif
