#pragma once

#include <Common.h>

typedef struct DVDFileInfo DVDFileInfo;

struct DVDFileInfo {
    u8 _00[0x34];
    u32 length;
    u8 _38[0x4];
};

BOOL DVDOpen(const char *fileName, DVDFileInfo *fileInfo);

s32 DVDReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio);

#define DVDRead(fileInfo, addr, length, offset) \
    DVDReadPrio((fileInfo), (addr), (length), (offset), 2)

BOOL DVDClose(DVDFileInfo *fileInfo);
