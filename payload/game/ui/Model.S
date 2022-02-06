#include <Common.S>

// Correctly load vehicle models in the "Change Ghost Data" section

PATCH_REPLACE_START(Model_805f5520, 0x2c)
    cmpwi r0, 0x4d
PATCH_REPLACE_END(Model_805f5520, 0x2c)
