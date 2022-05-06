#pragma once

#include "MultiDvdArchive.h"

extern const char *courseFilenames[0x28];

typedef struct {
    u8 _00[0x10 - 0x00];
    void *buffer;
    EGG_Heap *heap;
    u32 courseId;
    u32 state;
    MultiDvdArchive *multi;
} CourseCache;
static_assert(sizeof(CourseCache) == 0x24);

typedef struct {
    u8 _000[0x004 - 0x000];
    MultiDvdArchive **multiDvdArchives;
    u8 _008[0x588 - 0x008];
    CourseCache courseCache;
    u8 _5ac[0x61c - 0x5ac];
} ResourceManager;
static_assert(sizeof(ResourceManager) == 0x61c);

extern ResourceManager *s_resourceManager;

void ResourceManager_process(ResourceManager *this);

u16 ResourceManager_getMenuArchiveCount(ResourceManager *this);

void ResourceManager_preloadCourseAsync(ResourceManager *this, u32 courseId);
