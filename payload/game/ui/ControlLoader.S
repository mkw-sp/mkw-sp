#include <Common.S>

PATCH_BL_START(ControlLoader_load, 0x74)

stw  r3, 4(r27) // Original instruction
addi r4, r1, 8
bl ControlCheckValid

b ControlLoader_load + 0x78
PATCH_BL_END(ControlLoader_load, 0x74)
