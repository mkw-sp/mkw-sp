#include <Common.S>

PATCH_REPLACE_START(__OSThreadInit, 0x7C)
    lis       r4, main_thread_stack_pointer_top@ha
    lis       r5, main_thread_stack_pointer_bottom@ha
    lwz       r4, main_thread_stack_pointer_top@l(r4)
PATCH_REPLACE_END(__OSThreadInit, 0x7C)

PATCH_REPLACE_START(__OSThreadInit, 0x8C)
    lwz       r5, main_thread_stack_pointer_bottom@l(r5)
PATCH_REPLACE_END(__OSThreadInit, 0x8C)

PATCH_B_START(OSCreateThread, 0x0)
    b     OSCreateThread_RandomizeThreadStackPointer
PATCH_B_END(OSCreateThread, 0x0)

.globl j_OSCreateThread
j_OSCreateThread:
    stwu  r1, -0x20(r1) # Original instruction
    b     OSCreateThread + 0x4
