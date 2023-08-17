#include "../SectionId.S"

#include <Common.S>

PATCH_BL_START(CharacterSelectPage_onInit, 0x90)
    lwz r0, 0x0 (r3) // Original instruction

    // Check that the section is "Change Ghost Data"
    cmpwi r0, SingleChangeGhostData
    bnelr

    // Pretend it's "Change Course" to correctly set the previous page
    li r0, SingleChangeCourse

    b CharacterSelectPage_onInit + 0x94
PATCH_BL_END(CharacterSelectPage_onInit, 0x90)

// Exclude Change Ghost Data (repurposed MK Channel challenge ghost) from sections that load the MK Channel instead of SingleTop

PATCH_REPLACE_START(CharacterSelectPage_onActivate, 0x180)
    cmpwi r0, SingleChannelLeaderboardChallenge
PATCH_REPLACE_END(CharacterSelectPage_onActivate, 0x180)

PATCH_REPLACE_START(CharacterSelectPage_8083dfa8, 0x564)
    // Check the SP bits for team mode
    rlwinm. r3, r0, 0, 0, 1
PATCH_REPLACE_END(CharacterSelectPage_8083dfa8, 0x564)
