#include "MultiDvdArchive.h"

#include "../host_system/SystemManager.h"

#include <stdio.h>
#include <string.h>

#include <revolution.h>

enum {
    RESOURCE_KIND_FILE_DOUBLE_FORMAT = 0x0,
    RESOURCE_KIND_FILE_SINGLE_FORMAT = 0x1,
};

extern const char *languageSuffixes[];

// Change the number of menu archives to 6
PATCH_S16(MultiDvdArchive_create, 0xf2, 6);

static void my_MenuMultiDvdArchive_init(MenuMultiDvdArchive *this) {
    const char *languageSuffix = languageSuffixes[s_systemManager->language];
    if (REGION == REGION_K) {
        snprintf(this->names[0], 0x80, "_R.szs");
    } else {
        snprintf(this->names[0], 0x80, ".szs");
    }
    snprintf(this->names[1], 0x80, "%s", languageSuffix);
    snprintf(this->names[2], 0x80, "SP.szs");
    snprintf(this->names[3], 0x80, "SP%s", languageSuffix);
    snprintf(this->names[4], 0x80, "_Dif.szs");
    snprintf(this->names[5], 0x80, "%.2s_Dif.szs", languageSuffix);

    for (u32 i = 0; i < 6; i++) {
        this->kinds[i] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    }
}
PATCH_B(MenuMultiDvdArchive_init, my_MenuMultiDvdArchive_init);
