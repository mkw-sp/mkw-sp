#include "dvdex.h"

#include <revolution.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum {
    MAX_PATH_LENGTH = 47, // /Race/Competition/CommonObj/CommonObj01.szs is the longest with 44
    MAX_FILE_COUNT = 3072, // About 2000 in the base game
};

static OSMutex mutex;
static u32 fileCount = 0;
static char paths[MAX_FILE_COUNT][MAX_PATH_LENGTH + 1];

void DVDExInit(void) {
    OSInitMutex(&mutex);
}

BOOL DVDExOpen(const char *fileName, DVDFileInfo *fileInfo) {
    assert(fileName);
    assert(fileInfo);

    char path[strlen("/mkw-sp/disc") + MAX_PATH_LENGTH + 1];
    snprintf(path, sizeof(path), "/mkw-sp/disc%s", fileName);

    if (!Storage_open(&fileInfo->cb.file, path, MODE_READ)) {
        return false;
    }

    fileInfo->startAddr = 0;
    fileInfo->length = Storage_size(&fileInfo->cb.file);
    fileInfo->callback = NULL;
    fileInfo->cb.state = 0;
    fileInfo->cb.command = (u32)-1; // Mark this as a replaced file

    return true;
}

s32 DVDExReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 UNUSED(prio)) {
    assert(fileInfo);
    assert(addr);
    assert(!((u32)addr & 0x1f));
    assert(length < INT32_MAX);
    assert(!(length & 0x1f));
    assert(offset >= 0);
    assert(!(offset & 3));

    if (!Storage_lseek(&fileInfo->cb.file, offset)) {
        return false;
    }

    u32 readLength;
    return Storage_read(&fileInfo->cb.file, addr, length, &readLength) ? (s32)readLength : -1;
}

BOOL DVDExReadAsyncPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset,
        DVDCallback callback, s32 prio) {
    s32 result = DVDExReadPrio(fileInfo, addr, length, offset, prio);
    if (callback) {
        callback(result, (DVDFileInfo *)fileInfo);
    }
    return true;
}

BOOL DVDExClose(DVDFileInfo *fileInfo) {
    assert(fileInfo);

    return Storage_close(&fileInfo->cb.file);
}

s32 DVDExConvertPathToEntrynum(const char *pathPtr) {
    OSLockMutex(&mutex);

    if (strlen(pathPtr) > MAX_PATH_LENGTH) {
        assert(false && "[DVDEX] Exceeded maximum path length!\n");
    }

    for (u32 i = 0; i < fileCount; i++) {
        if (!strcmp(paths[i], pathPtr)) {
            OSUnlockMutex(&mutex);
            return i;
        }
    }

    if (fileCount >= MAX_FILE_COUNT) {
        assert(false && "[DVDEX] Exceeded maximum file count!\n");
    }

    s32 i = fileCount;
    snprintf(paths[fileCount++], MAX_PATH_LENGTH + 1, "%s", pathPtr);
    OSUnlockMutex(&mutex);
    return i;
}

const char *DVDExConvertEntrynumToPath(s32 entrynum) {
    assert(entrynum >= 0 && (u32)entrynum < fileCount);

    return paths[entrynum];
}
