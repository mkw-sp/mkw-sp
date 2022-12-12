#include <Common.S>

#define MAIN_THREAD_STACK_BITS_ENTROPY 11

PATCH_B_START(__init_registers, 0x8C)
    mftb      r0
    addi      r3, r1, 7
    clrlwi    r1, r0, 32 - MAIN_THREAD_STACK_BITS_ENTROPY
    subf      r1, r1, r3
    clrrwi    r1, r1, 3

    li        r0, 0
    li        r3, 0

    blr
PATCH_B_END(__init_registers, 0x8C)
