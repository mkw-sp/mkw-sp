#include <Common.S>

.global StackTrace_TryDecryptLinkRegister
StackTrace_TryDecryptLinkRegister:
    cmpwi     r6, 0
    blt-      .LdecryptedLinkRegister

    lis       r7, __stack_chk_guard@ha
    lwz       r7, __stack_chk_guard@l(r7)
    xor       r6, r6, r7

.LdecryptedLinkRegister:
    crclr     4*cr1+eq # Original instruction
    blr
