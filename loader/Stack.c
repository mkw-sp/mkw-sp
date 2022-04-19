#include "Cache.h"

#include <sp/security/Stack.h>

#define MAIN_THREAD_STACK_BITS_ENTROPY 11

void Stack_RandomizeMainThreadStackPointer(void)
{
    const void* __init_registers = (void*)0x80006210;

    u16* lis = (u16*)((u8*)__init_registers + 0x76);
    u16* ori = (u16*)((u8*)__init_registers + 0x7A);

    u32* stack_pointer = (u32*)((*lis << 16) | (*ori << 0));
    stack_pointer = Stack_RandomizeStackPointer(stack_pointer, MAIN_THREAD_STACK_BITS_ENTROPY);

    *lis = (u32)stack_pointer >> 16;
    *ori = (u32)stack_pointer >>  0;

    DCFlushRange(lis, sizeof(*lis));
    DCFlushRange(ori, sizeof(*ori));
    ICInvalidateRange(lis, sizeof(*lis));
    ICInvalidateRange(ori, sizeof(*ori));
}
