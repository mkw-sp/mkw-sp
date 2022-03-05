#include "dvdex.h"

#include <revolution.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

enum {
    MAX_PREFIX_COUNT = 32,
    MAX_PREFIX_LENGTH = 32,
    MAX_PATH_LENGTH = 47, // /Race/Competition/CommonObj/CommonObj01.szs is the longest with 44
    MAX_FILE_COUNT = 3072, // About 2000 in the base game
};

static u32 prefixCount = 0;
static wchar_t prefixes[MAX_PREFIX_COUNT][MAX_PREFIX_LENGTH + 1];
static OSMutex mutex;
static u32 fileCount = 0;
static char paths[MAX_FILE_COUNT][MAX_PATH_LENGTH + 1];

static void discoverMyStuffPrefixes(void) {
    Dir dir;
    if (!Storage_openDir(&dir, L"/mkw-sp")) {
        return;
    }

    DirEntry entry;
    while (Storage_readDir(&dir, &entry)) {
        if (entry.type == NODE_TYPE_NONE) {
            break;
        }
        if (entry.type != NODE_TYPE_DIR) {
            continue;
        }
        if (wcslen(entry.name) > MAX_PREFIX_LENGTH) {
            continue;
        }
        if (wcsncmp(entry.name, L"My Stuff", wcslen(L"My Stuff"))) {
            continue;
        }
        swprintf(prefixes[prefixCount++], MAX_PREFIX_LENGTH + 1, L"%ls", entry.name);
        SP_LOG("Added file replacement prefix %ls", entry.name);
    }

    Storage_closeDir(&dir);
}

void DVDExInit(void) {
    swprintf(prefixes[prefixCount++], MAX_PREFIX_LENGTH + 1, L"disc");
    discoverMyStuffPrefixes();

    OSInitMutex(&mutex);
}

static bool tryOpen(const wchar_t *path, DVDFileInfo *fileInfo) {
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

BOOL DVDExOpen(const char *fileName, DVDFileInfo *fileInfo) {
    assert(fileName);
    assert(fileInfo);

    if (fileName[0] == '/') {
        fileName++;
    }
    for (u32 i = prefixCount; i --> 0;) {
        wchar_t path[MAX_PREFIX_LENGTH + MAX_PATH_LENGTH + 1];
        swprintf(path, sizeof(path) / sizeof(wchar_t), L"/mkw-sp/%ls/%s", prefixes[i], fileName);
        if (tryOpen(path, fileInfo)) {
            return true;
        }
        const char *bare = fileName;
        for (const char *s = fileName; *s != '\0'; s++) {
            if (*s == '/') {
                bare = s + 1;
            }
        }
        if (bare == fileName) {
            continue;
        }
        swprintf(path, sizeof(path) / sizeof(wchar_t), L"/mkw-sp/%ls/%s", prefixes[i], bare);
        if (tryOpen(path, fileInfo)) {
            return true;
        }
    }

    return false;
}

s32 DVDExReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 UNUSED(prio)) {
    assert(fileInfo);
    assert(addr);
    assert(!((u32)addr & 0x1f));
    assert(length < INT32_MAX);
    assert(!(length & 0x1f));
    assert(offset >= 0);
    assert(!(offset & 3));

    u32 readLength;
    if (!Storage_read(&fileInfo->cb.file, addr, length, offset, &readLength)) {
        return -1;
    }

    if (offset + readLength == Storage_size(&fileInfo->cb.file)) {
        readLength = OSRoundUp32B(readLength);
    }
    return (s32)readLength;
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
