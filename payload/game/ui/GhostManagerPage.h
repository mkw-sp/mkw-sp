#pragma once

#include "../system/GhostFile.h"

typedef struct {
    GhostFile *file;
    u8 _04[0x08 - 0x04];
    u32 index;
    u8 _0c[0x10 - 0x0c];
} GhostEntry;

typedef struct {
    GhostEntry entries[GHOST_TYPE_MAX];
    u32 count;
    u8 _264[0x268 - 0x264];
} GhostList;

GhostFile *GhostList_getFile(GhostList *this, u32 index);

enum {
    GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_RACE = 0x3,
    GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_REPLAY = 0x6,
};

typedef struct {
    u8 _0000[0x009c - 0x0000];
    u32 nextRequest;
    GhostList list;
    u8 _0308[0x2b0c - 0x0308];
} GhostManagerPage;

void GhostManagerPage_setupGhostReplay(GhostManagerPage *this, bool isStaffGhost);

void GhostManagerPage_setupGhostRace(GhostManagerPage *this, bool isStaffGhost, bool isNewRecord, bool fromReplay);
