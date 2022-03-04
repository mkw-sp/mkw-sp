#include <Common.S>

PATCH_B_START(OSCreateThread, 0x0)
    b     OSCreateThread_RandomizeThreadStackPointer
PATCH_B_END(OSCreateThread, 0x0)

.globl j_OSCreateThread
j_OSCreateThread:
    stwu  r1, -0x20(r1) # Original instruction
    b     OSCreateThread + 0x4
