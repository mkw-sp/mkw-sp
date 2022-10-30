#pragma once

#include <Common.h>

extern u32 __stack_chk_guard;

__attribute__((no_stack_protector)) void Stack_InitCanary(void);

void Stack_RelocateMainThreadStackToMEM1End(void);

void Stack_DoLinkRegisterPatches(u32 *startAddress, u32 *endAddress);

static inline u32 *Stack_RandomizeStackPointer(u32 *sp, u32 bitsEntropy) {
    return (u32 *)((((u32)sp - (__builtin_ppc_mftb() & ((1 << bitsEntropy) - 1))) + 0x7) & ~0x7);
}

static inline bool Stack_IsLinkRegisterEncrypted(u32 *lr) {
    return ((u32)lr & (1 << 31)) == 0;
}

static inline u32 *Stack_XORLinkRegister(u32 *lr) {
    return (u32 *)((u32)lr ^ __stack_chk_guard);
}
