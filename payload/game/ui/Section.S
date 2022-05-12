#include <Common.S>

PATCH_B_START(Section_init, 0xec)
    // Use MEM1 for nw4r lyt allocations also for the menu scene (on top of the globe scene).
    li r4, 1
    slwi r3, r4, r3
    andi. r3, r3, 0x12

    b Section_init + 0xf0
PATCH_B_END(Section_init, 0xec)
