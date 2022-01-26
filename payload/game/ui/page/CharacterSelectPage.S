#include <Common.S>

PATCH_REPLACE_START(CharacterSelectPage_onInit, 0xa4)
    // Load the time trial rules page when coming from a race
    li r0, 0x6a
PATCH_REPLACE_END(CharacterSelectPage_onInit, 0xa4)
