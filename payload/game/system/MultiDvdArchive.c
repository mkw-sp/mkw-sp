#include "MultiDvdArchive.h"

#include "../host_system/SystemManager.h"

#include <stdio.h>
#include <string.h>

#include <revolution.h>

enum {
    RESOURCE_KIND_FILE_DOUBLE_FORMAT = 0x0,
    RESOURCE_KIND_FILE_SINGLE_FORMAT = 0x1,
    RESOURCE_KIND_FILE_SHORT_FORMAT = 0x3,
};

extern const char *languageSuffixes[];

// Change the number of menu archives to 6
PATCH_S16(MultiDvdArchive_create, 0xf2, 6);

static const char *filenames[9] = {
    "/Scene/UI/Award",
    "/Scene/UI/Channel",
    "/Scene/UI/Event",
    "/Scene/UI/Globe",
    "/Scene/UI/MenuMulti",
    "/Scene/UI/MenuOther",
    "/Scene/UI/MenuSingle",
    "/Scene/UI/Present",
    "/Scene/UI/Title",
};

static const char *replacements[9] = {
    "/Scene/UI/Awrd",
    "/Scene/UI/Chan",
    "/Scene/UI/Evnt",
    "/Scene/UI/Glob",
    "/Scene/UI/Mult",
    "/Scene/UI/Othr",
    "/Scene/UI/Sngl",
    "/Scene/UI/Prsn",
    "/Scene/UI/Titl",
};

void MultiDvdArchive_formatShort(MultiDvdArchive *this, const char *filename, u32 i, char *path) {
    for (u32 j = 0; j < 9; j++) {
        if (!strcmp(filename, filenames[j])) {
            snprintf(path, 0x100, "%s%s", replacements[j], this->names[i]);
            return;
        }
    }

    snprintf(path, 0x100, "%s%s", filename, this->names[i]);
}

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
        OSReport("%s\n", this->names[i]);
    }

    this->kinds[0] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    this->kinds[1] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    this->kinds[2] = RESOURCE_KIND_FILE_SHORT_FORMAT;
    this->kinds[3] = RESOURCE_KIND_FILE_SHORT_FORMAT;
    this->kinds[4] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
    this->kinds[5] = RESOURCE_KIND_FILE_DOUBLE_FORMAT;
}
PATCH_B(MenuMultiDvdArchive_init, my_MenuMultiDvdArchive_init);
