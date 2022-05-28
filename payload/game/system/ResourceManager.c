#include "ResourceManager.h"

#include "game/system/RootScene.h"

#include <revolution.h>

#include <stdio.h>

enum {
    COURSE_CACHE_STATE_CLEARED = 0,
    COURSE_CACHE_STATE_LOADED = 2,
};

static void CourseCache_init(CourseCache *self) {
    if (!self->heap) {
        assert(!self->buffer);
        EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
        self->buffer = spAlloc(0xb00000 /* 11 MiB */, -0x20, heap);
        self->heap = &EGG_ExpHeap_create2(self->buffer, 0xb00000, 1)->base;
        self->state = COURSE_CACHE_STATE_CLEARED;
    }
}

static void CourseCache_deinit(CourseCache *self) {
    if (self->heap) {
        assert(self->buffer);
        self->state = COURSE_CACHE_STATE_CLEARED;
        EGG_ExpHeap_destroy((EGG_ExpHeap *)self->heap);
        self->heap = NULL;
        self->buffer = NULL;
    }
}

static void CourseCache_load2(CourseCache *self, u32 courseId, bool splitScreen) {
    self->courseId = courseId;
    if (self->state == COURSE_CACHE_STATE_LOADED) {
        MultiDvdArchive_clear(self->multi);
    }
    char path[128];
    if (splitScreen) {
        snprintf(path, sizeof(path), "Race/Course/%s_d", courseFilenames[courseId]);
    } else {
        snprintf(path, sizeof(path), "Race/Course/%s", courseFilenames[courseId]);
    }
    MultiDvdArchive_load(self->multi, path, self->heap, self->heap, 0);
    assert(MultiDvdArchive_isLoaded(self->multi));
    self->state = COURSE_CACHE_STATE_LOADED;
}

void CourseCache_load(CourseCache *self, u32 courseId);

static void my_CourseCache_load(CourseCache *self, u32 courseId) {
    CourseCache_load2(self, courseId, false);
}
PATCH_B(CourseCache_load, my_CourseCache_load);

MultiDvdArchive *my_ResourceManager_loadCourse(ResourceManager *self, u32 courseId, EGG_Heap *heap,
        bool splitScreen) {
    if (self->courseCache.state != COURSE_CACHE_STATE_LOADED ||
            self->courseCache.courseId != courseId) {
        CourseCache_load2(&self->courseCache, courseId, splitScreen);
    }
    MultiDvdArchive *multi = self->multis[MULTI_DVD_ARCHIVE_TYPE_COURSE];
    MultiDvdArchive_loadOther(multi, self->courseCache.multi, heap);
    self->courseCache.state = COURSE_CACHE_STATE_CLEARED;
    return multi;
}
PATCH_B(ResourceManager_loadCourse, my_ResourceManager_loadCourse);

static u16 my_ResourceManager_getMenuArchiveCount(ResourceManager *self) {
    const MultiDvdArchive *multi = self->multis[MULTI_DVD_ARCHIVE_TYPE_MENU];
    u16 loadedCount = 0;
    for (u16 i = 0; i < multi->archiveCount; i++) {
        if (multi->archives[i].state == DVD_ARCHIVE_STATE_MOUNTED) {
            loadedCount++;
        }
    }
    return loadedCount;
}
PATCH_B(ResourceManager_getMenuArchiveCount, my_ResourceManager_getMenuArchiveCount);

DvdArchive *ResourceManager_getMenuArchive(ResourceManager *self, u32 i) {
    MultiDvdArchive *multi = self->multis[MULTI_DVD_ARCHIVE_TYPE_MENU];
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

static void ResourceManager_initGlobeHeap(ResourceManager *self) {
    if (!self->globeHeap) {
        SP_LOG("gh i");
        EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
        size_t size = OSRoundUp32B(0x107d080 + 0x20400);
        void *buffer = spAlloc(size, -0x20, heap);
        self->globeHeap = &EGG_ExpHeap_create2(buffer, size, 1)->base;
        SP_LOG("%p", self->globeHeap);
    }
}

static void ResourceManager_deinitGlobeHeap(ResourceManager *self) {
    if (self->globeHeap) {
        SP_LOG("gh d");
        EGG_ExpHeap_destroy((EGG_ExpHeap *)self->globeHeap);
        self->globeHeap = NULL;
        self->globeLoadingIsBusy = false;
    }
}

void ResourceManager_onCreateScene(u32 sceneId) {
    switch (sceneId) {
    case SCENE_ID_MENU:
    case SCENE_ID_RACE:
        ResourceManager_deinitGlobeHeap(s_resourceManager);
        CourseCache_init(&s_resourceManager->courseCache);
        break;
    case SCENE_ID_GLOBE:
        CourseCache_deinit(&s_resourceManager->courseCache);
        ResourceManager_initGlobeHeap(s_resourceManager);
        break;
    }
}
