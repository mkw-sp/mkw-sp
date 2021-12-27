#pragma once

#include <Common.h>

typedef struct OSModuleInfo OSModuleInfo;
typedef struct OSModuleHeader OSModuleHeader;
typedef struct OSSectionInfo OSSectionInfo;
typedef struct OSImportInfo OSImportInfo;

struct OSModuleInfo {
    u8 _00[0x0c];
    u32 numSections;
    u32 sectionInfoOffset;
    u8 _14[0x0c];
};

struct OSModuleHeader {
    OSModuleInfo info;
    u8 _20[0x08];
    u32 impOffset;
    u32 impSize;
    u8 prologSection;
    u8 _31[0x03];
    u32 prolog;
    u8 _38[0x10];
    u32 fixSize;
};

struct OSSectionInfo {
    u32 offset;
    u32 size;
};

struct OSImportInfo {
    u8 _0[0x4];
    u32 offset;
};

// Not actually exposed in the API
void Relocate(OSModuleHeader *existingModule, OSModuleHeader *newModule);
