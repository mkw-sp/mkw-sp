#pragma once

#include "Page.h"

typedef struct {
    u8 _000[0x008 - 0x000];
    Page *pages[PAGE_ID_MAX];
    u8 _354[0x408 - 0x354];
} Section;
static_assert(sizeof(Section) == 0x408);

void Section_createPage(u32 pageId);
