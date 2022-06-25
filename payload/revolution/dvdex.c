#include "dvdex.h"

#include <revolution.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

enum {
    MAX_PREFIX_COUNT = 32,
    MAX_PREFIX_LENGTH = 32,
    MAX_PATH_LENGTH = 51, // /Race/Competition/CommonObj/CommonObj01.szs is the longest with 43
};

static u32 prefixCount = 0;
static wchar_t prefixes[MAX_PREFIX_COUNT][MAX_PREFIX_LENGTH + 1];

static void discoverMyStuffPrefixes(void) {
    Dir dir;
    if (!Storage_openDir(&dir, L"/mkw-sp")) {
        return;
    }

    NodeInfo info;
    while (Storage_readDir(&dir, &info)) {
        if (info.type == NODE_TYPE_NONE) {
            break;
        }
        if (info.type != NODE_TYPE_DIR) {
            continue;
        }
        if (wcslen(info.name) > MAX_PREFIX_LENGTH) {
            continue;
        }
        if (wcsncmp(info.name, L"My Stuff", wcslen(L"My Stuff"))) {
            continue;
        }
        swprintf(prefixes[prefixCount++], MAX_PREFIX_LENGTH + 1, L"%ls", info.name);
        SP_LOG("Added file replacement prefix %ls", info.name);
    }

    Storage_closeDir(&dir);
}

void DVDExInit(void) {
    discoverMyStuffPrefixes();
}

static bool tryOpen(const wchar_t *path, DVDFileInfo *fileInfo) {
    size_t length = wcslen(path);
    if (length >= wcslen(L".szs") && !wcscmp(path + length - wcslen(L".szs"), L".szs")) {
        wchar_t lzmaPath[MAX_PREFIX_LENGTH + MAX_PATH_LENGTH + 1];
        swprintf(lzmaPath, ARRAY_SIZE(lzmaPath), L"%.*ls.arc.lzma", length - wcslen(L".szs"), path);
        if (tryOpen(lzmaPath, fileInfo)) {
            return true;
        }
    }

    if (!Storage_open(&fileInfo->cb.file, path, "r")) {
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
    wchar_t path[MAX_PREFIX_LENGTH + MAX_PATH_LENGTH + 1];
    swprintf(path, sizeof(path) / sizeof(wchar_t), L"nas:/%s", fileName);
    return tryOpen(path, fileInfo);
}

s32 DVDExReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 UNUSED(prio)) {
    assert(fileInfo);
    assert(addr);
    assert(!((u32)addr & 0x1f));
    assert(length < INT32_MAX);
    assert(!(length & 0x1f));
    assert(offset >= 0);
    assert(!(offset & 3));

    u32 fileLength = Storage_size(&fileInfo->cb.file);
    if (fileLength < (u32)offset + (u32)length) {
        length = fileLength - offset;
        if (length < 0) {
            length = 0;
        }
    }

    if (!Storage_read(&fileInfo->cb.file, addr, length, offset)) {
        return -1;
    }

    if ((u32)offset + (u32)length == Storage_size(&fileInfo->cb.file)) {
        length = OSRoundUp32B(length);
    }
    return length;
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

s32 DVDExConvertPathToEntrynum(const char *fileName) {
    assert(fileName);

    DVDFileInfo fileInfo;
    if (!DVDOpen(fileName, &fileInfo)) {
        return -1;
    }

    DVDClose(&fileInfo);
    return 0;
}
