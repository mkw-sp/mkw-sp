#include "RaceMenuPage.h"

bool GhostReplayPauseMenuPage_vf_74;

static bool my_GhostReplayPauseMenuPage_vf_74(void) {
    // Allow closing with option
    return true;
}
PATCH_B(GhostReplayPauseMenuPage_vf_74, my_GhostReplayPauseMenuPage_vf_74);

static u32 afterTaButtons[] = {
    0x3,
    0x5,
    0x6,
    0x4,
    0x1,
};

u32 AfterTAMenuPage_getButtonCount(void);

static u32 my_AfterTAMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(afterTaButtons);
}
PATCH_B(AfterTAMenuPage_getButtonCount, my_AfterTAMenuPage_getButtonCount);

const u32 *AfterTAMenuPage_getButtons(void);

static const u32 *my_AfterTAMenuPage_getButtons(void) {
    return afterTaButtons;
}
PATCH_B(AfterTAMenuPage_getButtons, my_AfterTAMenuPage_getButtons);

static u32 vsPauseButtons[] = {
    0x0,
    0x2,
    0x1,
};

u32 VsPauseMenuPage_getButtonCount(void);

static u32 my_VsPauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(vsPauseButtons);
}
PATCH_B(VsPauseMenuPage_getButtonCount, my_VsPauseMenuPage_getButtonCount);

const u32 *VsPauseMenuPage_getButtons(void);

static const u32 *my_VsPauseMenuPage_getButtons(void) {
    return vsPauseButtons;
}
PATCH_B(VsPauseMenuPage_getButtons, my_VsPauseMenuPage_getButtons);

const char *VsPauseMenuPage_getFile(void);

static const char *my_VsPauseMenuPage_getFile(void) {
    return "PauseMenuVS";
}
PATCH_B(VsPauseMenuPage_getFile, my_VsPauseMenuPage_getFile);

static u32 battlePauseButtons[] = {
    0x0,
    0x2,
    0x1,
};

u32 BattlePauseMenuPage_getButtonCount(void);

static u32 my_BattlePauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(battlePauseButtons);
}
PATCH_B(BattlePauseMenuPage_getButtonCount, my_BattlePauseMenuPage_getButtonCount);

const u32 *BattlePauseMenuPage_getButtons(void);

static const u32 *my_BattlePauseMenuPage_getButtons(void) {
    return battlePauseButtons;
}
PATCH_B(BattlePauseMenuPage_getButtons, my_BattlePauseMenuPage_getButtons);

const char *BattlePauseMenuPage_getFile(void);

static const char *my_BattlePauseMenuPage_getFile(void) {
    return "PauseMenuVS";
}
PATCH_B(BattlePauseMenuPage_getFile, my_BattlePauseMenuPage_getFile);
