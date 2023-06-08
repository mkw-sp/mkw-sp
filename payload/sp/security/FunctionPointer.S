#include <Common.S>

// This function assumes that r12 contains the function pointer
.globl FunctionPointer_CheckFunctionPointer
FunctionPointer_CheckFunctionPointer:
    lis       r11, dol_text_start@ha
    addi      r11, r11, dol_text_start@l
    cmplw     r12, r11
    blt-      LinvalidAddress

    lis       r11, dol_text_end@ha
    addi      r11, r11, dol_text_end@l
    cmplw     r12, r11
    bltctr-

    lis       r11, rel_text_start@ha
    addi      r11, r11, rel_text_start@l
    cmplw     r12, r11
    blt-      LinvalidAddress

    lis       r11, rel_text_end@ha
    addi      r11, r11, rel_text_end@l
    cmplw     r12, r11
    bltctr+

    lis       r11, payload_text_start@ha
    addi      r11, r11, payload_text_start@l
    cmplw     r12, r11
    blt-      LinvalidAddress

    lis       r11, payload_replacements_end@ha
    addi      r11, r11, payload_replacements_end@l
    cmplw     r12, r11
    bltctr+

LinvalidAddress:
    lis       r3, LinvalidAddressMessage@ha
    addi      r3, r3, LinvalidAddressMessage@l
    crclr     4*cr1+eq
    bl        panic


.section .rodata

LinvalidAddressMessage:
    .asciz "Function pointer subterfuge detected!"
