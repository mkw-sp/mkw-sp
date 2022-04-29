#include <Common.S>

// This function can be better optimized if we can find another register that is free for use
.globl Stack_XORAlignedLinkRegisterSave
Stack_XORAlignedLinkRegisterSave:
    mtctr     r11

    lis       r11, __stack_chk_guard@ha
    lwz       r11, __stack_chk_guard@l(r11)
    xor       r0, r0, r11
    stw       r0, 4(r12)

    mfctr     r11
    blr

.globl Stack_XORAlignedLinkRegisterRestore
Stack_XORAlignedLinkRegisterRestore:
    lis       r12, __stack_chk_guard@ha
    lwz       r11, 4(r10)
    lwz       r12, __stack_chk_guard@l(r12)
    xor       r0, r11, r12
    blr

// This function can be better optimized if we can find another register that is free for use
.globl Stack_XORLinkRegisterSave
Stack_XORLinkRegisterSave:
    mtctr     r11

    lis       r11, __stack_chk_guard@ha
    lwz       r11, __stack_chk_guard@l(r11)
    xor       r0, r0, r11
    lwz       r11, 0(r1)
    stw       r0, 4(r11)

    mfctr     r11
    blr

.globl Stack_XORLinkRegisterRestore
Stack_XORLinkRegisterRestore:
    lwz       r11, 0(r1)
    lis       r12, __stack_chk_guard@ha
    lwz       r12, __stack_chk_guard@l(r12)
    lwz       r11, 4(r11)
    xor       r0, r11, r12
    blr
