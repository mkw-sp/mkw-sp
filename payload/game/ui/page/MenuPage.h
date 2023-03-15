#pragma once

#include "../Page.h"

#include "../ctrl/CtrlMenuInstructionText.h"

typedef struct {
    Page inherit;
    MiiGroup *miiGroup;
    u8 _048[0x2bc - 0x048];
    CtrlMenuInstructionText *instructionText;
    u8 _2c0[0x3ec - 0x2c0];
    s32 prevId;
    u8 _3f0[0x430 - 0x3f0];
} MenuPage;
static_assert(sizeof(MenuPage) == 0x430);
