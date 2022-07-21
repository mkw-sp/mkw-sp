#include <revolution/os.h>
#include <sp/security/Stack.h>

#define THREAD_STACK_SIZE_RIGHT_SHIFT_AMOUNT 3

extern BOOL j_OSCreateThread(OSThread *thread, void *(*func)(void *), void *param, void *stack, u32 stackSize, s32 priority, u16 attr);

BOOL OSCreateThread_RandomizeThreadStackPointer(OSThread *thread, void *(*func)(void *), void *param, void *stack, u32 stackSize, s32 priority, u16 attr)
{
    void* old_stack_base = stack;
    u32   old_stack_size = stackSize;

    assert(old_stack_size >> THREAD_STACK_SIZE_RIGHT_SHIFT_AMOUNT != 0);

    u32* new_stack_base = Stack_RandomizeStackPointer((u32*)old_stack_base, __builtin_ctz(old_stack_size >> THREAD_STACK_SIZE_RIGHT_SHIFT_AMOUNT));
    u32  new_stack_size = old_stack_size - ((u32)old_stack_base - (u32)new_stack_base);

    if ((SP_DEBUG_LEVEL & SP_DEBUG_STACK_RANDOMIZE) == SP_DEBUG_STACK_RANDOMIZE) {
        OSReport("--------------------------------\n");
        OSReport("[MEM] OSCreateThread: %p"      "\n", (void *)thread);
        OSReport("[MEM] old_stack_base: %p"      "\n", (void *)old_stack_base);
        OSReport("[MEM] old_stack_size: 0x%08X"  "\n", old_stack_size);
        OSReport("[MEM] new_stack_base: %p"      "\n", (void *)new_stack_base);
        OSReport("[MEM] new_stack_size: 0x%08X"  "\n", new_stack_size);
        OSReport("--------------------------------\n");
    }

    return j_OSCreateThread(thread, func, param, new_stack_base, new_stack_size, priority, attr);
}
