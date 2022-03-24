#pragma once

#include <Common.h>

__attribute__((no_stack_protector)) void Stack_InitCanary(void);

static inline u32* Stack_RandomizeStackPointer(u32* stack_pointer, u32 bits_entropy)
{
    return (u32*)((((u32)stack_pointer - (__builtin_ppc_mftb() & ((1 << bits_entropy) - 1))) + 0x7) & ~0x7);
}
