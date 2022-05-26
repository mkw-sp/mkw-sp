#pragma once

#include <Common.h>

extern u32 __stack_chk_guard;

__attribute__((no_stack_protector)) void Stack_InitCanary(void);

void Stack_RelocateMainThreadStackToMEM1End(void);

void Stack_DoLinkRegisterPatches(u32* start_address, u32* end_address);

static inline u32* Stack_RandomizeStackPointer(u32* stack_pointer, u32 bits_entropy)
{
    return (u32*)((((u32)stack_pointer - (__builtin_ppc_mftb() & ((1 << bits_entropy) - 1))) + 0x7) & ~0x7);
}

static inline bool Stack_IsLinkRegisterEncrypted(u32* link_register)
{
    return ((u32)link_register & (1 << 31)) == 0;
}

static inline u32* Stack_XORLinkRegister(u32* link_register)
{
    return (u32*)((u32)link_register ^ __stack_chk_guard);
}
