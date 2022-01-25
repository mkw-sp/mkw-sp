#pragma once

#include "MultiDvdArchive.h"

extern const char *courseFilenames[0x28];

typedef struct {
    u8 _000[0x004 - 0x000];
    MultiDvdArchive **multiDvdArchives;
    u8 _008[0x61c - 0x008];
} ResourceManager;
static_assert(sizeof(ResourceManager) == 0x61c);

u16 ResourceManager_getMenuArchiveCount(ResourceManager *this);
