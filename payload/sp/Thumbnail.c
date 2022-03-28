#include "Thumbnail.h"

#include "game/host_system/SystemManager.h"
#include "sp/Storage.h"

#include <egg/core/eggSystem.h>

#include <stdio.h>
#include <string.h>
#include <wchar.h>

char currentName[256];
bool hasDir;
Dir dir;
NodeInfo info;
wchar_t path[256];
wchar_t fileName[256];

wchar_t *Thumbnail_getCoursePath(const char *name) {
    const char *suffix = "_Dif.szs";
    size_t length = strlen(name);
    if (length >= strlen(suffix) && !strcmp(&name[length - strlen(suffix)], suffix)) {
        return NULL;
    }

    swprintf(fileName, ARRAY_SIZE(fileName), L"_");
    if (strcmp(name, currentName)) {
        if (hasDir) {
            Storage_closeDir(&dir);
            hasDir = false;
        }
        snprintf(currentName, ARRAY_SIZE(currentName), "%s", name);
        swprintf(path, ARRAY_SIZE(path), L"/mkw-sp/slots/%s", name);
        if (!Storage_openDir(&dir, path)) {
            return NULL;
        }
        if (!Storage_readDir(&dir, &info) || info.type != NODE_TYPE_FILE) {
            Storage_closeDir(&dir);
            return NULL;
        }
        hasDir = true;
        return NULL;
    }

    if (!hasDir) {
        return NULL;
    }
    swprintf(fileName, ARRAY_SIZE(fileName), L"%ls", info.name);
    swprintf(path, ARRAY_SIZE(path), L"/mkw-sp/slots/%s/%ls", name, info.name);
    if (!Storage_readDir(&dir, &info) || info.type != NODE_TYPE_FILE) {
        Storage_closeDir(&dir);
        hasDir = false;
    }
    return path;
}

void Thumbnail_takeScreenshot(void) {
    s32 offset = swprintf(path, ARRAY_SIZE(path), L"/mkw-sp/screenshots");
    if (offset < 0 || (size_t)offset > ARRAY_SIZE(path)) {
        return;
    }
    if (!Storage_createDir(path, true)) {
        return;
    }

    s32 length = swprintf(path + offset, ARRAY_SIZE(path) - offset, L"/%s", currentName);
    offset += length;
    if (length < 0 || (size_t)offset > ARRAY_SIZE(path)) {
        return;
    }
    if (!Storage_createDir(path, true)) {
        return;
    }

    length = swprintf(path + offset, ARRAY_SIZE(path) - offset, L"/%ls", fileName);
    offset += length;
    if (length < 0 || (size_t)offset > ARRAY_SIZE(path)) {
        return;
    }
    if (!Storage_createDir(path, true)) {
        return;
    }

    const char *aspectName = s_systemManager->aspectRatio ? "169" : "43";
    length = swprintf(path + offset, ARRAY_SIZE(path) - offset, L"/%s", aspectName);
    offset += length;
    if (length < 0 || (size_t)offset > ARRAY_SIZE(path)) {
        return;
    }
    if (!Storage_createDir(path, true)) {
        return;
    }

    for (u32 i = 0; i < 1024; i++) {
        length = swprintf(path + offset, ARRAY_SIZE(path) - offset, L"/%d.xfb", i);
        if (length < 0 || (size_t)(offset + length) > ARRAY_SIZE(path)) {
            return;
        }
        const EGG_Xfb *xfb = sRKSystem.xfbManager->headXfb;
        u32 size = EGG_Xfb_calcXfbSize(xfb->width, xfb->height);
        if (Storage_writeFile(path, false, xfb->buffer, size)) {
            break;
        }
    }
}

bool Thumbnail_hasNext(void) {
    return hasDir;
}
