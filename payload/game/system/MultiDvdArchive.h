#pragma once

#include "DvdArchive.h"

enum {
    MULTI_DVD_ARCHIVE_TYPE_RACE = 0x0,
    MULTI_DVD_ARCHIVE_TYPE_COURSE = 0x1,
    MULTI_DVD_ARCHIVE_TYPE_MENU = 0x2,
    MULTI_DVD_ARCHIVE_TYPE_FONT = 0x3,
};

enum {
    RESOURCE_KIND_FILE_DOUBLE_FORMAT = 0x0,
    RESOURCE_KIND_FILE_SINGLE_FORMAT = 0x1,
};

typedef struct {
    const struct MultiDvdArchive_vt *vt;
    DvdArchive *archives;
    u16 archiveCount;
    u8 _0a[0x10 - 0x0a];
    char **names;
    u8 _14[0x18 - 0x14];
    u32 *kinds;
} MultiDvdArchive;
static_assert(sizeof(MultiDvdArchive) == 0x1c);

typedef struct MultiDvdArchive_vt {
    u8 _0[0x8 - 0x0];
    void (*dt)(MultiDvdArchive *this, s32 type);
    void (*init)(MultiDvdArchive *this);
} MultiDvdArchive_vt;
static_assert(sizeof(MultiDvdArchive_vt) == 0x10);

MultiDvdArchive *MultiDvdArchive_create(u32 type);

bool MultiDvdArchive_isLoaded(const MultiDvdArchive *self);

void MultiDvdArchive_load(MultiDvdArchive *self, const char *path, EGG_Heap *archiveHeap,
        EGG_Heap *fileHeap, u32 unused);

void MultiDvdArchive_loadOther(MultiDvdArchive *self, MultiDvdArchive *other, EGG_Heap *heap);

void MultiDvdArchive_clear(MultiDvdArchive *self);

typedef struct {
    MultiDvdArchive;
} RaceMultiDvdArchive;
static_assert(sizeof(RaceMultiDvdArchive) == 0x1c);

typedef struct {
    MultiDvdArchive;
} CourseMultiDvdArchive;
static_assert(sizeof(CourseMultiDvdArchive) == 0x1c);

typedef struct {
    MultiDvdArchive;
} MenuMultiDvdArchive;
static_assert(sizeof(MenuMultiDvdArchive) == 0x1c);

typedef struct {
    MultiDvdArchive;
} FontMultiDvdArchive;
