#include <Common.S>

.globl StackCanary_SaveAlignedLinkRegister
StackCanary_SaveAlignedLinkRegister:
    mtctr     r11

    mfspr     r11, IABR
    xor       r0, r11, r0
    stw       r0, 4(r12)

    mfctr     r11
    blr

.globl StackCanary_RestoreAlignedLinkRegister
StackCanary_RestoreAlignedLinkRegister:
    lwz       r11, 4(r10)
    mfspr     r12, IABR
    xor       r0, r12, r11
    blr

.globl StackCanary_SaveLinkRegister
StackCanary_SaveLinkRegister:
    mtctr     r11

    mfspr     r11, IABR
    xor       r0, r11, r0
    lwz       r11, 0(r1)
    stw       r0, 4(r11)

    mfctr     r11
    blr

.globl StackCanary_RestoreLinkRegister
StackCanary_RestoreLinkRegister:
    lwz       r11, 0(r1)
    mfspr     r12, IABR
    lwz       r11, 4(r11)
    xor       r0, r12, r11
    blr
