#pragma once

#include <Common.h>

void StackCanary_Init(void);

static inline bool StackCanary_IsLinkRegisterEncrypted(u32 lr) {
    return (lr & (1 << 31)) == 0;
}

static inline u32 StackCanary_XORLinkRegister(u32 lr) {
    u32 iabr;
    asm("mfspr %0, 1010" : "=r"(iabr));

    return lr ^ iabr;
}
