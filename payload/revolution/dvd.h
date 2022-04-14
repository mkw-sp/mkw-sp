#pragma once

#include "sp/Storage.h"

typedef struct DVDCommandBlock DVDCommandBlock;
typedef struct DVDFileInfo DVDFileInfo;

typedef void (*DVDCallback)(s32 result, DVDFileInfo *fileInfo);

struct DVDCommandBlock {
    union {
        struct {
            DVDCommandBlock *next;
            DVDCommandBlock *prev;
        };
        struct { // Added
            File file;
        };
    };
    u32 command;
    s32 state;
    u8 _10[0x30 - 0x10];
};
static_assert_32bit(sizeof(DVDCommandBlock) == 0x30);

struct DVDFileInfo {
    DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    DVDCallback callback;
};
static_assert_32bit(sizeof(DVDFileInfo) == 0x3c);

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
static_assert_32bit(sizeof(DVDDirEntry) == 0xc);

BOOL DVDOpen(const char *fileName, DVDFileInfo *fileInfo);

BOOL DVDFastOpen(s32 entrynum, DVDFileInfo *fileInfo);

s32 DVDReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio);

#define DVDRead(fileInfo, addr, length, offset) \
    DVDReadPrio((fileInfo), (addr), (length), (offset), 2)

BOOL DVDClose(DVDFileInfo *fileInfo);

s32 DVDConvertPathToEntrynum(const char *fileName);

BOOL DVDFastOpenDir(s32 entrynum, DVDDir *dir);

BOOL DVDReadDir(DVDDir *dir, DVDDirEntry *dirent);

BOOL DVDCloseDir(DVDDir *dir);

s32 DVDCancel(DVDCommandBlock *block);
