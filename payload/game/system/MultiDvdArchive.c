#include "MultiDvdArchive.h"

#include "../host_system/SystemManager.h"

#include <stdio.h>

enum {
    RESOURCE_KIND_FILE_DOUBLE_FORMAT = 0x0,
    RESOURCE_KIND_FILE_SINGLE_FORMAT = 0x1,
};

extern const char *languageSuffixes[7];

// Change the number of menu archives to 4
PATCH_S16(MultiDvdArchive_create, 0xf2, 4);

static void my_MenuMultiDvdArchive_init(MenuMultiDvdArchive *this) {
    const char *languageSuffix = languageSuffixes[s_systemManager->language];
    snprintf(this->names[0], 0x80, ".szs");
    snprintf(this->names[1], 0x80, "%s", languageSuffix);
    snprintf(this->names[2], 0x80, "SP.szs");
    snprintf(this->names[3], 0x80, "SP%s", languageSuffix);

    for (u32 i = 0; i < 4; i++) {
        this->kinds[i] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    }
}
PATCH_B(MenuMultiDvdArchive_init, my_MenuMultiDvdArchive_init);
