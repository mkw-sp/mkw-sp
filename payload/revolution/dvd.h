#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x34 - 0x00];
    u32 length;
    u8 _38[0x3c - 0x38];
} DVDFileInfo;
static_assert(sizeof(DVDFileInfo) == 0x3c);

typedef struct {
    u32 entryNum;
    u32 location;
    u32 next;
} DVDDir;
static_assert(sizeof(DVDDir) == 0xc);

typedef struct {
    u32 entryNum;
    BOOL isDir;
    char *name;
} DVDDirEntry;
static_assert(sizeof(DVDDirEntry) == 0xc);

BOOL DVDOpen(const char *fileName, DVDFileInfo *fileInfo);

BOOL DVDFastOpen(s32 entrynum, DVDFileInfo *fileInfo);

s32 DVDReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio);

#define DVDRead(fileInfo, addr, length, offset) \
    DVDReadPrio((fileInfo), (addr), (length), (offset), 2)

BOOL DVDClose(DVDFileInfo *fileInfo);

s32 DVDConvertPathToEntrynum(const char *pathPtr);

BOOL DVDFastOpenDir(s32 entrynum, DVDDir *dir);

BOOL DVDReadDir(DVDDir *dir, DVDDirEntry *dirent);

BOOL DVDCloseDir(DVDDir *dir);
