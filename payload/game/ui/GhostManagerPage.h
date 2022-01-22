#pragma once

#include "../system/GhostFile.h"

typedef struct {
    const GhostFile *file;
    u32 groupIndex;
    u32 index;
    bool isNew;
} GhostEntry;
static_assert(sizeof(GhostEntry) == 0x10);

struct GhostManagerPage;

typedef struct {
    GhostEntry entries[GHOST_TYPE_MAX];
    u32 count;
    struct GhostManagerPage *ghostManagerPage;
} GhostList;
static_assert(sizeof(GhostList) == 0x268);

void GhostList_populate(GhostList *this, u32 courseId);

GhostFile *GhostList_getFile(GhostList *this, u32 index);

enum {
    GHOST_MANAGER_PAGE_REQUEST_NONE = 0x0,
    GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_RACE = 0x3,
    GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_REPLAY = 0x6,
    GHOST_MANAGER_PAGE_REQUEST_POPULATE = 0xe, // Added
};

typedef struct GhostManagerPage {
    u8 _0000[0x0054 - 0x0000];
    GhostGroup *groups[8];
    u8 _0074[0x0094 - 0x0074];
    u32 currentRequest;
    u8 _0098[0x009c - 0x0098];
    u32 nextRequest;
    GhostList list;
    u8 _0308[0x2b0c - 0x0308];
} GhostManagerPage;
static_assert(sizeof(GhostManagerPage) == 0x2b0c);

void GhostManagerPage_requestPopulate(GhostManagerPage *this);
