#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x0c - 0x00];
    s32 state;
    u8 _10[0x30 - 0x10];
} DVDCommandBlock;
static_assert(sizeof(DVDCommandBlock) == 0x30);

typedef struct {
    DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    void *callback;
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

REPLACE BOOL DVDOpen(const char *fileName, DVDFileInfo *fileInfo);

BOOL DVDFastOpen(s32 entrynum, DVDFileInfo *fileInfo);

s32 DVDReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio);

#define DVDRead(fileInfo, addr, length, offset) \
    DVDReadPrio((fileInfo), (addr), (length), (offset), 2)

BOOL DVDClose(DVDFileInfo *fileInfo);

s32 DVDConvertPathToEntrynum(const char *fileName);

BOOL DVDOpenDir(const char *dirName, DVDDir *dir);

BOOL DVDFastOpenDir(s32 entrynum, DVDDir *dir);

BOOL DVDReadDir(DVDDir *dir, DVDDirEntry *dirent);

BOOL DVDCloseDir(DVDDir *dir);

s32 DVDCancel(DVDCommandBlock *block);

s32 DVDGetEntrynum(const DVDFileInfo *fileInfo);

s32 DVDGetDirEntrynum(const DVDDir *dir);

void DVDExClone(const DVDFileInfo *src, DVDFileInfo *dst);

void DVDExCloneDir(const DVDDir *src, DVDDir *dst);
