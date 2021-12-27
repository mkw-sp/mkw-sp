#pragma once

#include "GhostFile.h"

#include <egg/core/eggTaskThread.h>

#include <revolution.h>

enum {
    MAX_SP_LICENSE_COUNT = 4,
};

typedef struct {
    u32 magic;
    u32 size;
    u32 version;
    RawMii mii;
} SpLicense;

typedef struct {
    u8 _00000[0x00014 - 0x00000];
    void *rawSave;
    u8 *rawGhostFile;
    EGG_TaskThread *taskThread;
    u32 ghostCount; // Modified
    RawGhostHeader *rawGhostHeaders; // Modified
    GhostGroup *ghostGroup;
    char (*ghostPaths)[NAND_MAX_PATH]; // Modified
    u8 _00030[0x24ffc - 0x00030];
    void *otherRawSave;
    bool isBusy;
    bool isValid;
    bool canSave;
    u8 _25003[0x25004 - 0x25003];
    u32 result;
    SpLicense *spLicenses[MAX_SP_LICENSE_COUNT];
} SaveManager;

extern SaveManager *s_saveManager;

SaveManager *SaveManager_createInstance(void);

void SaveManager_initAsync(SaveManager *this);

void SaveManager_resetAsync(SaveManager *this);

void SaveManager_loadGhostAsync(SaveManager *this, s32 licenseId, u32 category, u32 index, u32 courseId);

void SaveManager_loadGhostHeadersAsync(SaveManager *this, s32 licenseId, GhostGroup *group);
