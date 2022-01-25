#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x34 - 0x00];
    u32 length;
    u8 _38[0x3c - 0x38];
} DVDFileInfo;
static_assert(sizeof(DVDFileInfo) == 0x3c);

BOOL DVDOpen(const char *fileName, DVDFileInfo *fileInfo);

s32 DVDReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio);

#define DVDRead(fileInfo, addr, length, offset) \
    DVDReadPrio((fileInfo), (addr), (length), (offset), 2)

BOOL DVDClose(DVDFileInfo *fileInfo);
