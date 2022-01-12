#pragma once

#include "../Page.h"

typedef struct {
    Page;
    MiiGroup *miiGroup;
    u8 _048[0x430 - 0x48];
} MenuPage;
static_assert(sizeof(MenuPage) == 0x430);
