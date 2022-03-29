#pragma once

#include "game/system/GhostFile.h"

enum {
    GHOST_MANAGER_PAGE_REQUEST_NONE = 0x0,
    GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_RACE = 0x3,
    GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_REPLAY = 0x6,
    GHOST_MANAGER_PAGE_REQUEST_POPULATE = 0xe, // Added
};

typedef struct {
    u8 _000[0x268 - 0x000];
} GhostList;
static_assert(sizeof(GhostList) == 0x268);

typedef struct {
    u16 count;
    u16 indices[MAX_GHOST_COUNT];
} SpGhostList;

typedef struct {
    u8 _0000[0x0054 - 0x0000];
    GhostGroup *groups[8];
    u8 _0074[0x0094 - 0x0074];
    u32 currentRequest;
    u8 _0098[0x009c - 0x0098];
    u32 nextRequest;
    u8 _00a0[0x2b0c - 0x00a0];
    SpGhostList list; // Added
} GhostManagerPage;
static_assert(offsetof(GhostManagerPage, list) == 0x2b0c);

void GhostManagerPage_requestPopulate(GhostManagerPage *this);
