#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "hal_filesystem.h"
#include "pcf.h"

#define PCF_PROPERTIES          (1<<0)
#define PCF_ACCELERATORS        (1<<1)
#define PCF_METRICS             (1<<2)
#define PCF_BITMAPS             (1<<3)
#define PCF_INK_METRICS         (1<<4)
#define PCF_BDF_ENCODINGS       (1<<5)
#define PCF_SWIDTHS             (1<<6)
#define PCF_GLYPH_NAMES         (1<<7)
#define PCF_BDF_ACCELERATORS    (1<<8)

#define PCF_DEFAULT_FORMAT      0x00000000
#define PCF_INKBOUNDS           0x00000200
#define PCF_ACCEL_W_INKBOUNDS   0x00000100
#define PCF_COMPRESSED_METRICS  0x00000100

#define PCF_BYTE_MASK           (1<<2)
#ifndef PCF_DEBUG
#define PCF_DEBUG 0
#endif
#define bread32_0(f) bread32(u32_0, f)
#define bread32_1(f) bread32(u32_1, f)
#define bread16_0(f) bread16(u16_0, f)
#define bread16_1(f) bread16(u16_1, f)

static uint8_t bread[4];

uint32_t u32_0(uint8_t * buf) {
	uint32_t result = 0;
	for (int i = 3; i >= 0; i--) {
		result <<= 8;
		result |= buf[i];
	}
	return result;
}

uint32_t u32_1(uint8_t * buf) {
	uint32_t result = 0;
	for (int i = 0; i < 4; i++) {
		result <<= 8;
		result |= buf[i];
	}
	return result;
}

uint16_t u16_0(uint8_t * buf) {
	uint16_t result = 0;
	for (int i = 1; i >= 0; i--) {
		result <<= 8;
		result |= buf[i];
	}
	return result;
}

uint16_t u16_1(uint8_t * buf) {
	uint16_t result = 0;
	for (int i = 0; i < 2; i++) {
		result <<= 8;
		result |= buf[i];
	}
	return result;
}

uint32_t bread32(uint32_t (*r)(), File * f) {
	hal_fs_read(f, bread, 4);
	return r(bread);
}

uint32_t bread16(uint16_t (*r)(), File * f) {
	hal_fs_read(f, bread, 2);
	return r(bread);
}

void pcf_encoding_load(encoding_table * t, File * f) {
#if (PCF_DEBUG)
	printf("%s offset: %d\n", __FUNCTION__, t->table->offset);
#endif
	if (hal_fs_seek(f, t->table->offset) < 0) { return; }
	t->format = bread32_0(f);
#if (PCF_DEBUG)
	printf("%s format: %04X %04X\n", __FUNCTION__, t->format, t->table->format);
#endif
	t->minCharOrByte2 = bread16_1(f);
	t->maxCharOrByte2 = bread16_1(f);
	t->minByte1 = bread16_1(f);
	t->maxByte1 = bread16_1(f);
	t->defChar = bread16_1(f);
	t->index_count = (t->maxCharOrByte2 - t->minCharOrByte2 + 1) * (t->maxByte1 - t->minByte1 + 1);
	/*int16_t * indexes = t->indexes = pvPortMalloc(sizeof(int16_t) * size);
	for (int i = 0; i < size; i++) {
		indexes[i] = bread16_1(f);
	}*/
	if ((t->maxCharOrByte2 - t->minCharOrByte2) == 0xff
			&& (t->maxByte1 - t->minByte1) == 0xff)
		t->direct = 1;
	else
		t->direct = 0;
#if (PCF_DEBUG)
	printf("encoding table: %d %d %d %d %d\n", t->minCharOrByte2, t->maxCharOrByte2, t->minByte1, t->maxByte1, t->index_count);
#endif
}

uint16_t pcf_encoding_index(encoding_table * t, File * f, uint32_t i) {
	if (hal_fs_seek(f, t->table->offset + 14 + i * 2) < 0) { return 0; }
	return bread16_1(f);
}

uint32_t pcf_encoding_find(encoding_table * t, File * f, uint32_t r) {
	if (t->direct) {
#if (PCF_DEBUG)
		printf("%s direct\n", __FUNCTION__);
#endif
		return pcf_encoding_index(t, f, r);
	}
	uint8_t b1 = 0xff & r, b2 = (r >> 8) & 0xff;
	uint32_t off = 0;
	if (b2 == 0) {
		off = b1 - t->minCharOrByte2;
	} else {
		off = (b2 - (uint32_t)t->minByte1) * (t->maxCharOrByte2 - t->minCharOrByte2 + 1)
			+ (b1 - t->minCharOrByte2);
	}
#if (PCF_DEBUG)
	printf("encoding_lookup0: %x %x %x %x\n", r, off, b1, b2);
	printf("encoding_lookup1: %x %x %x %x\n", t->minCharOrByte2, t->maxCharOrByte2, t->minByte1, t->maxByte1);
	printf("encoding_lookup3: %x %x\n", off, pcf_encoding_index(t, f, off));
#endif
	return pcf_encoding_index(t, f, off);
}

void pcf_metric_load(metric_table * t, File * f) {
#if (PCF_DEBUG)
	printf("%s offset: %d\n", __FUNCTION__, t->table->offset);
#endif
	if (hal_fs_seek(f, t->table->offset) < 0) { return; }
	t->format = bread32_0(f);
#if (PCF_DEBUG)
	printf("%s format: %04X %04X\n", __FUNCTION__, t->format, t->table->format);
#endif
	if (t->table->format & PCF_COMPRESSED_METRICS) {
		t->count = (t->table->format & PCF_BYTE_MASK)
			? bread16_1(f) : bread16_0(f);
#if (PCF_DEBUG)
		printf("%s compressed count: %d\n", __FUNCTION__, t->count);
#endif
	} else {
		t->count = (t->table->format & PCF_BYTE_MASK)
			? bread32_1(f) : bread32_0(f);
#if (PCF_DEBUG)
		printf("%s uncompressed count: %d\n", __FUNCTION__, t->count);
#endif
	}
}

void pcf_metric_find(metric_table * t, File * f, uint32_t i, metric_entry * me) {
#if (PCF_DEBUG)
	printf("%s i: %d, count: %d\n", __FUNCTION__, i, t->count);
#endif
	if (i > t->count) { return; }
	if (t->table->format & PCF_COMPRESSED_METRICS) {
		if (hal_fs_seek(f, (uint64_t)t->table->offset + 6 + (uint64_t)i * 5) < 0) { return; }
		uint8_t b[5];
		hal_fs_read(f, b, sizeof(b));
		me->left_sided_bearing = b[0];
		me->left_sided_bearing -= 0x80;
		me->right_side_bearing = b[1];
		me->right_side_bearing -= 0x80;
		me->character_width = b[2];
		me->character_width -= 0x80;
		me->character_ascent = b[3];
		me->character_ascent -= 0x80;
		me->character_descent = b[4];
		me->character_descent -= 0x80;
	} else {
		if (hal_fs_seek(f, (uint64_t)t->table->offset + 8 + (uint64_t)i * 12) < 0) { return; }
		uint16_t b[6];
		hal_fs_read(f, b, sizeof(b) * 2);
		me->left_sided_bearing = b[0];
		me->right_side_bearing = b[1];
		me->character_width = b[2];
		me->character_ascent = b[3];
		me->character_descent = b[4];
		me->character_attributes = b[5];
	}
}

void pcf_bitmap_load(bitmap_table * t, File * f) {
#if (PCF_DEBUG)
	printf("%s offset: %d\n", __FUNCTION__, t->table->offset);
#endif
	if (hal_fs_seek(f, t->table->offset) < 0) { return; }
	t->format = bread32_0(f);
#if (PCF_DEBUG)
	printf("%s format: %04X %04X\n", __FUNCTION__, t->format, t->table->format);
#endif
	t->count = bread32_1(f);
	/*int32_t * offsets = t->offsets = pvPortMalloc(sizeof(int32_t) * count);
	for (int i = 0; i < count; i++) {
		offsets[i] = bread32_1(f);
	}*/
	int32_t * bs = t->bitmapSizes;
	for (int i = 0; i < 4; i++) {
		bs[i] = bread32_1(f);
	}
#if (PCF_DEBUG)
	printf("bitmapSizes: %d %d %d %d, %x\n", bs[0], bs[1], bs[2], bs[3], t->format);
#endif
}

uint32_t pcf_bitmap_offset(bitmap_table * t, File * f, uint32_t i) {
	if (hal_fs_seek(f, t->table->offset + 8 + i * 4) < 0) { return 0; }
	return bread32_1(f);
}

void pcf_bitmap_find(bitmap_table * t, File * f, uint32_t i, bitmap * b) {
	if (i + 1 > t->count) { return; }
	uint64_t off = (uint64_t)t->table->offset + (uint64_t)(8 + 4 * t->count + 16);
	uint32_t offset_i = pcf_bitmap_offset(t, f, i);
	off += (uint64_t)offset_i;
	uint32_t size = pcf_bitmap_offset(t, f, i + 1) - pcf_bitmap_offset(t, f, i);
	if (size < 0) { return; }
	if (hal_fs_seek(f, off) < 0) { return; }
	uint8_t * data = b->data = pvPortMalloc(size);
	b->length = size;
	hal_fs_read(f, data, size);
}

void pcf_bitmap_free(bitmap * b) {
#if (PCF_DEBUG)
	printf("%s: %p\n", __FUNCTION__, b);
#endif
	vPortFree(b->data);
}

void pcf_open(pf * pf, char * path) {
	if (pf->init) { return; }
	pf->f = hal_fs_open(path, OM_READ);
	if (pf->f == NULL) { fprintf(stderr, "Failed to open the font.\n"); return; }
	file_header fh;
	hal_fs_read(pf->f, &fh, sizeof(fh));
#if (PCF_DEBUG)
	printf("%02X%02X%02X%02X\n", fh.header[0], fh.header[1], fh.header[2], fh.header[3]);
	printf("tableCount: %d\n", fh.table_count);
#endif
	toc_entry * toc_metrics, * toc_bitmaps, * toc_encoding;
	for (int i = 0; i < fh.table_count; i++) {
		toc_entry * toc = pvPortMalloc(sizeof(toc_entry)); // minor memory leak
		hal_fs_read(pf->f, toc, sizeof(toc_entry));
//		printf("toc->type:%d\n", toc->type);
		switch (toc->type) {
		case PCF_METRICS:
			toc_metrics = toc;
		case PCF_BITMAPS:
			toc_bitmaps = toc;
		case PCF_BDF_ENCODINGS:
			toc_encoding = toc;
		}
	}
	if (!toc_metrics || !toc_bitmaps) {
		return;
	}
	if (!toc_encoding) {
		return;
	}
	metric_table * mt = pf->metric = pvPortMalloc(sizeof(metric_table));
	mt->table = toc_metrics;
	pcf_metric_load(mt, pf->f);
#if (PCF_DEBUG)
	printf("metrics: %d\n", mt->count);
#endif
	bitmap_table * bt = pf->bitmap = pvPortMalloc(sizeof(bitmap_table));
	bt->table = toc_bitmaps;
	pcf_bitmap_load(bt, pf->f);
#if (PCF_DEBUG)
	printf("bitmaps: %d\n", bt->count);
#endif
	encoding_table * et = pf->encoding = pvPortMalloc(sizeof(encoding_table));
	et->table = toc_encoding;
	pcf_encoding_load(et, pf->f);
	pf->init = 1;
}

void pcf_free(pf * pf) {

}

void pcf_lookup(pf * pf, uint32_t r, bitmap * b, metric_entry * me) {
	uint32_t i = pcf_encoding_find(pf->encoding, pf->f, r);
	pcf_metric_find(pf->metric, pf->f, i, me);
	pcf_bitmap_find(pf->bitmap, pf->f, i, b);
//	*stride = 4;
}
