/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hal_filesystem.h"

#include "id3v2lib.h"


ID3v2_tag* load_tag(const char* file_name)
{
    File *file = NULL;
    ID3v2_tag *tag;

    // allocate buffer and fetch header
    if((file = hal_fs_open((char *)file_name, OM_READ)) == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    //parse free and return
    tag = load_tag_with_file(file);

    return tag;
}

// header is optional
ID3v2_tag* load_tag_with_file(File *file)
{
    // Declaration
    ID3v2_frame *frame;
    int offset = 0;
    ID3v2_tag* tag;
    ID3v2_header* tag_header;
    uint32_t skip = 0;
    int length = 0;

    // Initialization
    tag_header = get_tag_header_with_file(file);

    if(tag_header == NULL) // no valid header found
      return NULL;

    length = tag_header->tag_size + 10;

    if(get_tag_version(tag_header) == NO_COMPATIBLE_TAG)
    {
        // no supported id3 tag found
        vPortFree(tag_header);
        return NULL;
    }

    if(length < tag_header->tag_size+10)
    {
        // Not enough bytes provided to parse completely. TODO: how to communicate to the user the lack of bytes?
        vPortFree(tag_header);
        return NULL;
    }

    tag = new_tag();

    // Save file pointer
    tag->file = file;

    // Associations
    tag->tag_header = tag_header;

    // move the bytes pointer to the correct position
    skip+=10; // skip header
    if(tag_header->extended_header_size)
        // an extended header exists, so we skip it too
        skip+=tag_header->extended_header_size+4; // don't forget to skip the extended header size bytes too
    
    // we use tag_size here to prevent copying too much if the user provides more bytes than needed to this function

    while(offset < tag_header->tag_size)
    {
        frame = parse_frame(file, skip, offset, get_tag_version(tag_header));

        if(frame != NULL)
        {
            offset += frame->size + 10;
            add_to_list(tag->frames, frame);
        }
        else
        {
            break;
        }
    }

    return tag;
}

int get_tag_size(ID3v2_tag* tag)
{
    int size = 0;
    ID3v2_frame_list* frame_list = new_frame_list();

    if(tag->frames == NULL)
    {
        return size;
    }

    frame_list = tag->frames->start;
    while(frame_list != NULL)
    {
        size += frame_list->frame->size + 10;
        frame_list = frame_list->next;
    }

    return size;
}

/**
 * Getter functions
 */
ID3v2_frame* tag_get_title(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TIT2");
}

ID3v2_frame* tag_get_artist(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TPE1");
}

ID3v2_frame* tag_get_album(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TALB");
}

ID3v2_frame* tag_get_album_artist(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TPE2");
}

ID3v2_frame* tag_get_genre(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TCON");
}

ID3v2_frame* tag_get_track(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TRCK");
}

ID3v2_frame* tag_get_year(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TYER");
}

ID3v2_frame* tag_get_comment(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "COMM");
}

ID3v2_frame* tag_get_disc_number(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TPOS");
}

ID3v2_frame* tag_get_composer(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "TCOM");
}

ID3v2_frame* tag_get_album_cover(ID3v2_tag* tag)
{
    if(tag == NULL)
    {
        return NULL;
    }

    return get_from_list(tag->frames, "APIC");
}
