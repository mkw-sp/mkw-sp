#include "RaceMenuPage.h"

static const u32 ghostWatchPauseButtons[] = {
    0xc,
    0xf,
    0xe,
    0x12,
    0xd,
};

u32 GhostWatchPauseMenuPage_getButtonCount(void);

static u32 my_GhostWatchPauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(ghostWatchPauseButtons);
}
PATCH_B(GhostWatchPauseMenuPage_getButtonCount, my_GhostWatchPauseMenuPage_getButtonCount);

const u32 *GhostWatchPauseMenuPage_getButtons(void);

static const u32 *my_GhostWatchPauseMenuPage_getButtons(void) {
    return ghostWatchPauseButtons;
}
PATCH_B(GhostWatchPauseMenuPage_getButtons, my_GhostWatchPauseMenuPage_getButtons);

bool GhostWatchPauseMenuPage_vf_74(void);

static bool my_GhostWatchPauseMenuPage_vf_74(void) {
    // Allow closing with option
    return true;
}
PATCH_B(GhostWatchPauseMenuPage_vf_74, my_GhostWatchPauseMenuPage_vf_74);

static const u32 afterTaButtons[] = {
    0x3,
    0x12,
    0x5,
    0x6,
    0x4,
    0x1,
};

u32 AfterTimeAttackMenuPage_getButtonCount(void);

static u32 my_AfterTimeAttackMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(afterTaButtons);
}
PATCH_B(AfterTimeAttackMenuPage_getButtonCount, my_AfterTimeAttackMenuPage_getButtonCount);

const u32 *AfterTimeAttackMenuPage_getButtons(void);

static const u32 *my_AfterTimeAttackMenuPage_getButtons(void) {
    return afterTaButtons;
}
PATCH_B(AfterTimeAttackMenuPage_getButtons, my_AfterTimeAttackMenuPage_getButtons);

static const u32 vsPauseButtons[] = {
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

static const u32 battlePauseButtons[] = {
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

static const u32 timeAttackPauseButtons[] = {
    0x0,
    0x2,
    0x12,
    0x5,
    0x6,
    0x1,
};

u32 TimeAttackPauseMenuPage_getButtonCount(void);

static u32 my_TimeAttackPauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(timeAttackPauseButtons);
}
PATCH_B(TimeAttackPauseMenuPage_getButtonCount, my_TimeAttackPauseMenuPage_getButtonCount);

const u32 *TimeAttackPauseMenuPage_getButtons(void);

static const u32 *my_TimeAttackPauseMenuPage_getButtons(void) {
    return timeAttackPauseButtons;
}
PATCH_B(TimeAttackPauseMenuPage_getButtons, my_TimeAttackPauseMenuPage_getButtons);
