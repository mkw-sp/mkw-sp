#include <Common.S>

// Hide all controls

PATCH_REPLACE_START(RacePage_onInit, 0x3c)
    li r3, 0x0
PATCH_REPLACE_END(RacePage_onInit, 0x3c)



PATCH_B_START(RacePage_initInputs, 0x240)
    // Check if the section id is ghost replay
    cmpwi r0, 0x34
    bne .LinitInputs_end

    li r0, 0x6a // Pretend it's live view instead

.LinitInputs_end:
    cmpwi r0, 0x6a // Original instruction
    b RacePage_initInputs + 0x244
PATCH_B_END(RacePage_initInputs, 0x240)



// Set the number of CtrlRaceNameBalloon to 0

PATCH_REPLACE_START(RacePage_getControlCount, 0xc4)
    nop
PATCH_REPLACE_END(RacePage_getControlCount, 0xc4)



PATCH_B_START(RacePage_initControls, 0x1a0)
    bl CtrlRaceLap_load // Original instruction

    mr r3, r20
    mr r4, r19
    bl CtrlRaceLap_initLabelVisibility

    b RacePage_initControls + 0x1a4
PATCH_B_END(RacePage_initControls, 0x1a0)

PATCH_B_START(RacePage_initControls, 0x4c8)
    bl CtrlRaceTime_load // Original instruction

    mr r3, r14
    mr r4, r19
    bl CtrlRaceTime_initLabelVisibility

    b RacePage_initControls + 0x4cc
PATCH_B_END(RacePage_initControls, 0x4c8)



// Set the number of CtrlRaceNameBalloon to 0

PATCH_REPLACE_START(RacePage_initControls, 0x558)
    li r3, 0
PATCH_REPLACE_END(RacePage_initControls, 0x558)



// "GHOST DATA CANNOT BE SAVED" -> "Ghost data will be truncated."

PATCH_REPLACE_START(RacePage_initControls, 0x8d8)
    li r4, 10070
PATCH_REPLACE_END(RacePage_initControls, 0x8d8)



PATCH_B_START(RacePage_beforeCalc, 0x3c)
    // Check if the section id is live replay
    cmpwi r0, 0x34
    bne .LbeforeCalc_end

    // Check that the countdown has started
    lis r4, s_raceManager@ha
    lwz r4, s_raceManager@l (r4)
    lwz r4, 0x28 (r4)
    cmpwi r4, 0x0
    beq .LbeforeCalc_end

    li r0, 0x6a // If the section is ghost replay, pretend it's live view instead

.LbeforeCalc_end:
    cmpwi r0, 0x6a // Original instruction
    b RacePage_beforeCalc + 0x40
PATCH_B_END(RacePage_beforeCalc, 0x3c)



// Wait until the end of the race in ghost replay

PATCH_REPLACE_START(RacePage_canChangeSection, 0x14)
    nop
PATCH_REPLACE_END(RacePage_canChangeSection, 0x14)

PATCH_REPLACE_START(RacePage_canReplace, 0xd4)
    b 0x1c
PATCH_REPLACE_END(RacePage_canReplace, 0xd4)



// Always get the "FINISH" ending in time trials

PATCH_REPLACE_START(RacePage_beforeCalc, 0x380)
    b 0x34
PATCH_REPLACE_END(RacePage_beforeCalc, 0x380)



PATCH_B_START(RacePage_afterCalc, 0x1c0)
    bl Thumbnail_takeScreenshot

    bl Thumbnail_hasNext
    li r4, 0x4b
    mulli r3, r3, -0x31
    add r4, r4, r3
    mr r3, r27
    li r5, 0x0
    lfs f1, 0x0 (r31)
    lwz r12, 0x0 (r27)
    lwz r12, 0x1c (r12)
    mtctr r12
    bctrl

    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    li r4, 0x5
    stw r4, 0x1764 (r3)

    b RacePage_afterCalc + 0x340
PATCH_B_END(RacePage_afterCalc, 0x1c0)



// Do not show "Ghost data could not be saved." after 6 minutes

PATCH_REPLACE_START(RacePage_afterCalc, 0x5e8)
    nop
PATCH_REPLACE_END(RacePage_afterCalc, 0x5e8)



// Allow pausing during the countdown in non-replay modes

PATCH_REPLACE_START(RacePage_onPause, 0x70)
    b 0x20
PATCH_REPLACE_END(RacePage_onPause, 0x70)



// Also hide the HUD in ghost replay

PATCH_REPLACE_START(RacePage_onPause, 0x138)
    b 0x3c
PATCH_REPLACE_END(RacePage_onPause, 0x138)
