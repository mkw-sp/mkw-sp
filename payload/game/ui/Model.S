#include "SectionId.S"
#include <Common.S>

// Always use MEM1

PATCH_REPLACE_START(Model_805f5230, 0x1f8)
    b 0x20
PATCH_REPLACE_END(Model_805f5230, 0x1f8)



// Correctly load vehicle models in the "Change Ghost Data" section

PATCH_REPLACE_START(Model_805f5520, 0x2c)
    cmpwi r0, SingleChangeGhostData
PATCH_REPLACE_END(Model_805f5520, 0x2c)
