#include "Cache.h"

#include <sp/Stack.h>

#define MAIN_THREAD_STACK_BITS_ENTROPY 11

void RandomizeMainThreadStackPointer(void)
{
    const void* __init_registers = (void*)0x80006210;

    u16* lis = (u16*)((u8*)__init_registers + 0x76);
    u16* ori = (u16*)((u8*)__init_registers + 0x7A);

    u32* stack_pointer = (u32*)((*lis << 16) | (*ori << 0));
    stack_pointer = RandomizeStackPointer(stack_pointer, MAIN_THREAD_STACK_BITS_ENTROPY);

    *lis = (u32)stack_pointer >> 16;
    *ori = (u32)stack_pointer >>  0;

    DCFlushRange(lis, sizeof(u16));
    DCFlushRange(ori, sizeof(u16));
    ICInvalidateRange(lis, sizeof(u16));
    ICInvalidateRange(ori, sizeof(u16));
}
