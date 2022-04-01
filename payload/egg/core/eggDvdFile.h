#pragma once

#include <revolution.h>

typedef struct {
    u8 _00[0x04 - 0x00];
    bool isOpen;
    u8 _05[0x3c - 0x05];
    DVDFileInfo fileInfo;
    u8 _78[0xc8 - 0x78];
} EGG_DvdFile;
static_assert(sizeof(EGG_DvdFile) == 0xc8);

bool EGG_DvdFile_openPath(EGG_DvdFile *self, const char *path);
