#pragma once

#include <Common.h>

extern u32 __stack_chk_guard;

void StackCanary_Init(void);

static inline bool StackCanary_IsLinkRegisterEncrypted(u32 *lr) {
    return ((u32)lr & (1 << 31)) == 0;
}

static inline u32 *StackCanary_XORLinkRegister(u32 *lr) {
    return (u32 *)((u32)lr ^ __stack_chk_guard);
}
