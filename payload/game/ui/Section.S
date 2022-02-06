#include <Common.S>

// The game has 5 pages for the records, we only need 1 for the settings. Disable the 4 others.

PATCH_REPLACE_START(Section_addPages, 0xe28)
    nop
PATCH_REPLACE_END(Section_addPages, 0xe28)

PATCH_REPLACE_START(Section_addPages, 0xe34)
    nop
PATCH_REPLACE_END(Section_addPages, 0xe34)

PATCH_REPLACE_START(Section_addPages, 0xe40)
    nop
PATCH_REPLACE_END(Section_addPages, 0xe40)

PATCH_REPLACE_START(Section_addPages, 0xe4c)
    nop
PATCH_REPLACE_END(Section_addPages, 0xe4c)



// Complete the "Change Ghost Data" section (repurposed "Change Mission")

PATCH_B_START(Section_addActivePages, 0x784)
    mr r3, r31
    li r4, 0x70
    bl Section_addActivePage

    b Section_addActivePages + 0x14e0
PATCH_B_END(Section_addActivePages, 0x784)
