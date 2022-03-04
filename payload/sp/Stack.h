#pragma once

#include <Common.h>

static inline u32* RandomizeStackPointer(u32* stack_pointer, u32 bits_entropy)
{
    return (u32*)((((u32)stack_pointer - (__builtin_ppc_mftb() & ((1 << bits_entropy) - 1))) + 0x7) & ~0x7);
}
