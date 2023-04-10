#pragma once

#include "MenuPage.h"

#include "../Button.h"

typedef struct {
    MenuPage inherit;
    u8 _430[0x668 - 0x430];
    void *_668;
    u8 _66c[0x6b4 - 0x66c];
    u32 _6b4;
    void *_6b8;
    u8 _6bc[0xcb4 - 0x6bc];
    PushButton *fileAdminButton;
} TopMenuPage;
static_assert(sizeof(TopMenuPage) == 0xcb8);

TopMenuPage *TopMenuPage_ct(TopMenuPage *this);
