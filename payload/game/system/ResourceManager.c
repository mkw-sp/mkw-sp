#include "ResourceManager.h"

#include <stdio.h>

static u16 my_ResourceManager_getMenuArchiveCount(ResourceManager *this) {
    const MultiDvdArchive *multi = this->multiDvdArchives[MULTI_DVD_ARCHIVE_TYPE_MENU];
    u16 loadedCount = 0;
    for (u16 i = 0; i < multi->archiveCount; i++) {
        if (multi->archives[i].state == DVD_ARCHIVE_STATE_MOUNTED) {
            loadedCount++;
        }
    }
    return loadedCount;
}
PATCH_B(ResourceManager_getMenuArchiveCount, my_ResourceManager_getMenuArchiveCount);

DvdArchive *ResourceManager_getMenuArchive(ResourceManager *this, u32 i) {
    MultiDvdArchive *multi = this->multiDvdArchives[MULTI_DVD_ARCHIVE_TYPE_MENU];
    u16 loadedCount = 0;
    for (u16 j = 0; j < multi->archiveCount; j++) {
        if (multi->archives[j].state == DVD_ARCHIVE_STATE_MOUNTED) {
            if (loadedCount == i) {
                return &multi->archives[j];
            }
            loadedCount++;
        }
    }
    return NULL;
}

extern void ResourceManager_attachLayoutDir;

// Use the correct menu archive count to exit the loop
PATCH_S16(ResourceManager_attachLayoutDir, 0x42, 0xcc);

enum {
    COURSE_CACHE_STATE_CLEARED = 0,
    COURSE_CACHE_STATE_LOADED = 2,
};

void CourseCache_init(CourseCache *self);

static void my_CourseCache_init(CourseCache *self) {
    self->buffer = spAlloc(0x700000 /* 7 MiB */, -0x20, NULL);
    self->heap = &EGG_ExpHeap_create2(self->buffer, 0x700000, 1)->base;
}
PATCH_B(CourseCache_init, my_CourseCache_init);

void CourseCache_load(CourseCache *self, u32 courseId);

static void my_CourseCache_load(CourseCache *self, u32 courseId) {
    self->courseId = courseId;
    if (self->state == COURSE_CACHE_STATE_LOADED) {
        MultiDvdArchive_clear(self->multi);
    }
    char path[128];
    snprintf(path, sizeof(path), "Race/Course/%s", courseFilenames[courseId]);
    MultiDvdArchive_load(self->multi, path, self->heap, self->heap, 0);
    assert(MultiDvdArchive_isLoaded(self->multi));
    self->state = COURSE_CACHE_STATE_LOADED;
}
PATCH_B(CourseCache_load, my_CourseCache_load);
