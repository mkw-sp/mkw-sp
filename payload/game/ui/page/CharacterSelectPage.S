#include <Common.S>

PATCH_BL_START(CharacterSelectPage_onInit, 0x90)
    lwz r0, 0x0 (r3) // Original instruction

    // Check that the section is "Change Ghost Data"
    cmpwi r0, ChangeGhostData
    bnelr

    // Pretend it's "Change Course" to correctly set the previous page
    li r0, 0x4a

    b CharacterSelectPage_onInit + 0x94
PATCH_BL_END(CharacterSelectPage_onInit, 0x90)



PATCH_REPLACE_START(CharacterSelectPage_onInit, 0xa4)
    // Load the time trial rules page when coming from a race
    li r0, 0x6a
PATCH_REPLACE_END(CharacterSelectPage_onInit, 0xa4)

PATCH_REPLACE_START(CharacterSelectPage_8083e928, 0x27C)
    cmpwi r0, 0x4f
PATCH_REPLACE_END(CharacterSelectPage_8083e928, 0x27C)