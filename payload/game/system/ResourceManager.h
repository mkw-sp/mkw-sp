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
    MultiDvdArchive **multis;
    u8 _008[0x588 - 0x008];
    CourseCache courseCache;
    u8 _5ac[0x60c - 0x5ac];
    bool globeLoadingIsBusy;
    u8 _60d[0x614 - 0x60d];
    EGG_Heap *globeHeap;
    u8 _618[0x61c - 0x618];
} ResourceManager;
static_assert(sizeof(ResourceManager) == 0x61c);

extern ResourceManager *s_resourceManager;

void ResourceManager_process(ResourceManager *self);

MultiDvdArchive *ResourceManager_loadCourse(ResourceManager *self, u32 courseId, EGG_Heap *heap,
        bool splitScreen);

u16 ResourceManager_getMenuArchiveCount(ResourceManager *self);

void ResourceManager_preloadCourseAsync(ResourceManager *self, u32 courseId);

void ResourceManager_flush(ResourceManager *self);
