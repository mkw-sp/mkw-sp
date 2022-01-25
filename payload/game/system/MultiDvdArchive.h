#pragma once

#include "DvdArchive.h"

enum {
    MULTI_DVD_ARCHIVE_TYPE_MENU = 0x2,
};

typedef struct {
    u8 _00[0x04 - 0x00];
    DvdArchive *archives;
    u16 archiveCount;
    u8 _0a[0x10 - 0x0a];
    char **names;
    u8 _14[0x18 - 0x14];
    u32 *kinds;
} MultiDvdArchive;
static_assert(sizeof(MultiDvdArchive) == 0x1c);

MultiDvdArchive *MultiDvdArchive_create(u32 type);

typedef struct {
    MultiDvdArchive;
} MenuMultiDvdArchive;
static_assert(sizeof(MenuMultiDvdArchive) == 0x1c);

void MenuMultiDvdArchive_init(MenuMultiDvdArchive *this);
