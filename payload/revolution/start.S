#include <Common.S>

PATCH_REPLACE_START(__init_registers, 0x74)
    lis       r1, mainThreadSPTop@ha
    lwz       r1, mainThreadSPTop@l(r1)
PATCH_REPLACE_END(__init_registers, 0x74)
