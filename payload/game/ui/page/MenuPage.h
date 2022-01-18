#pragma once

#include "../Page.h"

typedef struct {
    Page;
    MiiGroup *miiGroup;
    u8 _048[0x3ec - 0x048];
    s32 prevId;
    u8 _3f0[0x430 - 0x3f0];
} MenuPage;
static_assert(sizeof(MenuPage) == 0x430);
