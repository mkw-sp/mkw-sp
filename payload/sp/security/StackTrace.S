#include <Common.S>

.global StackTrace_TryDecryptLinkRegister
StackTrace_TryDecryptLinkRegister:
    cmpwi     r6, 0
    blt-      .LdecryptedLinkRegister

    mfspr     r7, IABR
    xor       r6, r7, r6

.LdecryptedLinkRegister:
    crclr     4*cr1+eq # Original instruction
    blr
