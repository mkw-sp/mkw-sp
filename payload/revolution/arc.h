#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x08 - 0x00];
    int fstSize;
    int fileStart;
    u8 _10[0x20 - 0x10];
} ARCHeader;
static_assert(sizeof(ARCHeader) == 0x20);

typedef struct {
    u8 _00[0x04 - 0x00];
    void *FSTStart;
    u8 _08[0x0c - 0x08];
    u32 entryNum;
    u8 _10[0x1c - 0x10];
} ARCHandle;
static_assert(sizeof(ARCHandle) == 0x1c);

typedef struct {
    ARCHandle *handle;
    u32 startOffset;
    u32 length;
} ARCFileInfo;
static_assert(sizeof(ARCFileInfo) == 0xc);

typedef struct {
    ARCHandle *handle;
    u32 entryNum;
    u32 location;
    u32 next;
} ARCDir;
static_assert(sizeof(ARCDir) == 0x10);

typedef struct {
    u8 _00[0x04 - 0x00];
    u32 entryNum;
    BOOL isDir;
    char *name;
} ARCDirEntry;
static_assert(sizeof(ARCDirEntry) == 0x10);

BOOL ARCInitHandle(void *arcStart, ARCHandle *handle);

BOOL ARCOpen(ARCHandle *handle, const char *fileName, ARCFileInfo *af);

BOOL ARCFastOpen(ARCHandle *handle, s32 entrynum, ARCFileInfo *af);

s32 ARCConvertPathToEntrynum(ARCHandle *handle, const char *pathPtr);

u32 ARCGetStartOffset(ARCFileInfo *af);

u32 ARCGetLength(ARCFileInfo *af);

BOOL ARCClose(ARCFileInfo *af);

BOOL ARCOpenDir(ARCHandle *handle, const char *dirName, ARCDir *dir);

BOOL ARCFastOpenDir(ARCHandle *handle, s32 entrynum, ARCDir *dir); // Not in the SDK

BOOL ARCReadDir(ARCDir *dir, ARCDirEntry *dirent);

BOOL ARCCloseDir(ARCDir *dir);

void ARCExClone(const ARCFileInfo *src, ARCFileInfo *dst);

void ARCExCloneDir(const ARCDir *src, ARCDir *dst);
