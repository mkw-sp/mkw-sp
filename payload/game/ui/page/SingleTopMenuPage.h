#pragma once

#include <game/ui/page/MenuPage.h>

extern void SingleTopMenuPage_ct;
extern void SingleTopMenuPage_onActivate;
extern void SingleTopMenuPage_onButtonFront;
extern void SingleTopMenuPage_vf_64;
extern void SingleTopMenuPage_vf_84;

typedef struct {
    MenuPage;
    u8 _430[0x6c4 - 0x430];
} SingleTopMenuPage;
static_assert(sizeof(SingleTopMenuPage) == 0x6c4);

typedef enum {
    ButtonGP,
    ButtonTA,
    ButtonVS,
    ButtonBT,
    ButtonMR
} SingleTopButtonId;

