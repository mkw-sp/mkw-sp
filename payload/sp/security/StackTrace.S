#include <Common.S>

.global StackTrace_TryDecryptLinkRegister
StackTrace_TryDecryptLinkRegister:
    cmpwi     r6, 0
    blt-      .Ldecrypted_link_register

    lis       r7, __stack_chk_guard@ha
    lwz       r7, __stack_chk_guard@l(r7)
    xor       r6, r6, r7
    oris      r6, r6, 0x8000

.Ldecrypted_link_register:
    crclr     4*cr1+eq # Original instruction
    blr
