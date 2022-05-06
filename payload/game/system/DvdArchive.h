#pragma once

#include <egg/core/eggHeap.h>

enum {
    DVD_ARCHIVE_STATE_CLEARED = 0,
    DVD_ARCHIVE_STATE_RIPPED = 2,
    DVD_ARCHIVE_STATE_DECOMPRESSED = 3,
    DVD_ARCHIVE_STATE_MOUNTED = 4,
};

typedef struct {
    u8 _00[0x08 - 0x00];
    void *archiveBuffer;
    u32 archiveSize;
    EGG_Heap *archiveHeap;
    void *fileBuffer;
    u32 fileSize;
    EGG_Heap *fileHeap;
    u32 state;
} DvdArchive;
static_assert(sizeof(DvdArchive) == 0x24);

void DvdArchive_load(DvdArchive *self, const char *path, EGG_Heap *archiveHeap, bool isCompressed,
        s8 align, EGG_Heap *fileHeap, u32 unused);

void DvdArchive_rip(DvdArchive *self, const char *path, EGG_Heap *fileHeap, s8 align);
