#pragma once

#include "Page.h"

enum {
    SECTION_ID_TIME_ATTACK = 0x1f,
    SECTION_ID_GHOST_REPLAY = 0x34,
    SECTION_ID_MII_SELECT_CREATE = 0x45,
    SECTION_ID_MII_SELECT_CHANGE = 0x46,
};

typedef struct {
    u8 _000[0x008 - 0x000];
    Page *pages[PAGE_ID_MAX];
    u8 _354[0x408 - 0x354];
} Section;
static_assert(sizeof(Section) == 0x408);

void Section_createPage(u32 pageId);
