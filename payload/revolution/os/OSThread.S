#include <Common.S>

PATCH_REPLACE_START(__OSThreadInit, 0x7C)
    lis       r4, mainThreadSPTop@ha
    lis       r5, mainThreadSPBottom@ha
    lwz       r4, mainThreadSPTop@l(r4)
    lis       r3, 0xDEAE
    lwz       r5, mainThreadSPBottom@l(r5)
PATCH_REPLACE_END(__OSThreadInit, 0x7C)
