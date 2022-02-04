#include "ResourceManager.h"

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

DvdArchive *ResourceManager_getNthMenuArchive(ResourceManager *this, u32 i) {
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
