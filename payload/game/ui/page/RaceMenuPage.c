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
