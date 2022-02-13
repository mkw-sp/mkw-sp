#pragma once

#include "../Page.h"

typedef struct {
    Page_vt;
    void (* nextPage)(void* this, u32 pageId);
    // ...
} PausePageVT;

typedef struct {
    const PausePageVT *vt;
    PageBase;
    u8 _044[0x344 - 0x044];
} RaceMenuPage;
static_assert(sizeof(RaceMenuPage) == 0x344);
