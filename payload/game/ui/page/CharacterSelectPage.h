#pragma once

#include "MenuPage.h"

typedef struct {
    MenuPage;
    u8 _430[0x918 - 0x430];
} CharacterSelectPage;
static_assert(sizeof(CharacterSelectPage) == 0x918);
