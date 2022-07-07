#include <Common.S>

PATCH_B_START(TimeAttackTopPage_onActivate, 0xa0)
    lwz r0, 0x94 (r30) // Original instruction

    // Send our request, which might complete immediately
    mr r3, r30
    bl _ZN2UI16GhostManagerPage15requestPopulateEv
    mr r3, r30
    bl _ZN2UI16GhostManagerPage7processEv

    lwz r0, 0x94 (r30) // Original instruction again
    b TimeAttackTopPage_onActivate + 0xa4
PATCH_B_END(TimeAttackTopPage_onActivate, 0xa0)



PATCH_B_START(TimeAttackTopPage_onRefocus, 0xb4)
    lis r3, s_raceConfig@ha
    lwz r3, s_raceConfig@l (r3)
    addi r3, r3, 0x20 + 0xbf0
    bl RaceConfigScenario_resetGhostPlayerTypes

    lis r11, s_sectionManager@ha // Original instruction
    b TimeAttackTopPage_onRefocus + 0xb8
PATCH_B_END(TimeAttackTopPage_onRefocus, 0xb4)



// Move to the TimeAttackGhostListPage even with no ghost

PATCH_REPLACE_START(TimeAttackTopPage_onReady, 0x134)
    nop
PATCH_REPLACE_END(TimeAttackTopPage_onReady, 0x134)
