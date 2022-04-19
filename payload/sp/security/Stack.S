#include <Common.S>

// These functions can be better optimized if we can find another register that is free for use

.globl Stack_XORLinkRegisterRestore
Stack_XORLinkRegisterRestore:
    mtctr     r31

    lwz       r31, 0(r1)
    lwz       r0, 4(r31)

    lis       r31, __stack_chk_guard@ha
    lwz       r31, __stack_chk_guard@l(r31)

    xor       r0, r0, r31
    oris      r0, r0, 0x8000

    mfctr     r31
    blr

.globl Stack_XORLinkRegisterSave
Stack_XORLinkRegisterSave:
    mtctr     r31

    lis       r31, __stack_chk_guard@ha
    lwz       r31, __stack_chk_guard@l(r31)

    xor       r0, r0, r31
    clrlwi    r0, r0, 1 // Null terminator

    lwz       r31, 0(r1)
    stw       r0, 4(r31)

    mfctr     r31
    blr
