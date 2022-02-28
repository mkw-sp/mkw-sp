#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x0c - 0x00];
    u32 numSections;
    u32 sectionInfoOffset;
    u8 _14[0x20 - 0x14];
} OSModuleInfo;
static_assert(sizeof(OSModuleInfo) == 0x20);

typedef struct {
    OSModuleInfo info;
    u32 bssSize;
    u8 _24[0x28 - 0x24];
    u32 impOffset;
    u32 impSize;
    u8 prologSection;
    u8 _31[0x34 - 0x31];
    u32 prolog;
    u8 _38[0x48 - 0x38];
    u32 fixSize;
} OSModuleHeader;
static_assert(sizeof(OSModuleHeader) == 0x4c);

typedef struct {
    u32 offset;
    u32 size;
} OSSectionInfo;
static_assert(sizeof(OSSectionInfo) == 0x8);

typedef struct {
    u8 _0[0x4 - 0x0];
    u32 offset;
} OSImportInfo;
static_assert(sizeof(OSImportInfo) == 0x8);

// Not actually exposed in the API
void Relocate(OSModuleHeader *existingModule, OSModuleHeader *newModule);
