//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Derived from https://github.com/notxx/pcf
#include <stdint.h>

typedef struct bitmap {
	uint8_t * data;
	uint32_t length;
} bitmap;

typedef struct tocEntry {
	uint32_t type;	/* See below, indicates which table */
	uint32_t format;	/* See below, indicates how the data are formatted in the table */
	uint32_t size;	/* In bytes */
	uint32_t offset;	/* from start of file */
} toc_entry;

typedef struct fileHeader {
	char header[4];	/* always "\1fcp" */
	int table_count;
} file_header;

typedef struct metricEntry {
	int16_t left_sided_bearing;
	int16_t right_side_bearing;
	int16_t character_width;
	int16_t character_ascent;
	int16_t character_descent;
	uint16_t character_attributes;
} metric_entry;

typedef struct metricTable {
	toc_entry * table;
	int32_t format;
	int32_t count;
} metric_table;

typedef struct bitmapTable {
	toc_entry * table;
	int32_t format;
	int32_t count;
	int32_t bitmapSizes [4];
} bitmap_table;

typedef struct encodingTable {
	toc_entry * table;
	int32_t format;
	int16_t minCharOrByte2;
	int16_t maxCharOrByte2;
	int16_t minByte1;
	int16_t maxByte1;
	int16_t defChar;
	//int16_t * indexes;
	int32_t index_count;
	uint8_t direct;
} encoding_table;

typedef struct pf {
	encoding_table * encoding;
	bitmap_table * bitmap;
	metric_table *metric;
	FIL f;
	uint8_t init;
} pf;

void pcf_open(pf * pf, char * path);
void pcf_lookup(pf * pf, uint32_t r, bitmap * b, metric_entry * me);
void pcf_bitmap_free(bitmap * b);
void pcf_free(pf * pf);
