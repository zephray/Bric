/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"

#include "types.h"

ID3v2_tag* new_tag()
{
    ID3v2_tag* tag = (ID3v2_tag*) pvPortMalloc(sizeof(ID3v2_tag));
    if (!tag)
    	return NULL;
    tag->tag_header = new_header();
    if (!(tag->tag_header)) {
    	vPortFree(tag);
    	return NULL;
    }
    tag->frames = new_frame_list();
    if (!(tag->frames)) {
    	vPortFree(tag->tag_header);
    	vPortFree(tag);
    	return NULL;
    }
    return tag;
}

ID3v2_header* new_header()
{
    ID3v2_header* tag_header = (ID3v2_header*) pvPortMalloc(sizeof(ID3v2_header));
    if(tag_header != NULL)
    {
        memset(tag_header->tag, '\0', ID3_HEADER_TAG);
        tag_header->minor_version = 0x00;
        tag_header->major_version = 0x00;
        tag_header->flags = 0x00;
        memset(tag_header->tag, 0, ID3_HEADER_SIZE);
    }
    
    return tag_header;
}

ID3v2_frame* new_frame()
{
    ID3v2_frame* frame = (ID3v2_frame*) pvPortMalloc(sizeof(ID3v2_frame));
    return frame;
}

ID3v2_frame_list* new_frame_list()
{
    ID3v2_frame_list* list = (ID3v2_frame_list*) pvPortMalloc(sizeof(ID3v2_frame_list));
    if(list != NULL)
    {
        list->frame = NULL;
        list->next = NULL;
        list->start = NULL;
    }
    return list;
}

void free_tag(ID3v2_tag* tag)
{
    ID3v2_frame_list *list;

    vPortFree(tag->tag_header);
    list = tag->frames;
    while(list != NULL)
    {
        vPortFree(list->frame);
        list = list->next;
    }
    vPortFree(list);
    vPortFree(tag);
}

ID3v2_frame_text_content* new_text_content(int size)
{
    ID3v2_frame_text_content* content = (ID3v2_frame_text_content*) pvPortMalloc(sizeof(ID3v2_frame_text_content));
    if (!content)
    	return NULL;

    content->data = (char*) pvPortMalloc(size * sizeof(char));
    if (!(content->data)) {
    	vPortFree(content);
    	return NULL;
    }
    return content;
}

void free_text_content(ID3v2_frame_text_content* content) {
	vPortFree(content->data);
	vPortFree(content);
}

ID3v2_frame_comment_content* new_comment_content(int size)
{
    ID3v2_frame_comment_content* content = (ID3v2_frame_comment_content*) pvPortMalloc(sizeof(ID3v2_frame_comment_content));
    if (!content)
    	return NULL;
    content->text = new_text_content(size - ID3_FRAME_SHORT_DESCRIPTION - ID3_FRAME_LANGUAGE);
    if (!(content->text)) {
    	vPortFree(content);
    	return NULL;
    }
    content->language = (char*) pvPortMalloc(ID3_FRAME_LANGUAGE + sizeof(char));
    if (!(content->language)) {
    	vPortFree(content->text->data);
    	vPortFree(content->text);
    	vPortFree(content);
    	return NULL;
    }
    return content;
}

void free_comment_content(ID3v2_frame_comment_content* content) {
	vPortFree(content->language);
	vPortFree(content);
}

ID3v2_frame_apic_content* new_apic_content()
{
    ID3v2_frame_apic_content* content = (ID3v2_frame_apic_content*) pvPortMalloc(sizeof(ID3v2_frame_apic_content));
    return content;
}

void free_apic_content(ID3v2_frame_apic_content* content) {
	vPortFree(content->description);
	vPortFree(content->mime_type);
	vPortFree(content);
}
