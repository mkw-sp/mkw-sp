extern "C" {
#include "OSThread.h"
}

#define THREAD_STACK_SIZE_RIGHT_SHIFT_AMOUNT 3

extern "C" BOOL REPLACED(OSCreateThread)(OSThread *thread, void *(*func)(void *), void *param,
        void *stack, u32 stackSize, s32 priority, u16 attr);
extern "C" REPLACE BOOL OSCreateThread(OSThread *thread, void *(*func)(void *), void *param,
        void *stack, u32 stackSize, s32 priority, u16 attr) {
    void *oldStackBase = stack;
    u32 oldStackSize = stackSize;

    assert(oldStackSize >> THREAD_STACK_SIZE_RIGHT_SHIFT_AMOUNT != 0);
    u32 bitsEntropy = __builtin_ctz(oldStackSize >> THREAD_STACK_SIZE_RIGHT_SHIFT_AMOUNT);

    // clang-format off
    void *newStackBase =
            reinterpret_cast<void *>(
                ((reinterpret_cast<u32>(oldStackBase) - (__builtin_ppc_mftb() & ((1 << bitsEntropy) - 1))) + 0x7) & ~0x7
            );
    // clang-format on
    u32 newStackSize = oldStackSize -
            (reinterpret_cast<char *>(oldStackBase) - reinterpret_cast<char *>(newStackBase));

    if ((SP_DEBUG_LEVEL & SP_DEBUG_STACK_RANDOMIZE) == SP_DEBUG_STACK_RANDOMIZE) {
        // clang-format off
        OSReport("--------------------------------\n");
        OSReport("[MEM] OSCreateThread: %p"      "\n", reinterpret_cast<void *>(thread));
        OSReport("[MEM] oldStackBase  : %p"      "\n", oldStackBase);
        OSReport("[MEM] oldStackSize  : 0x%08x"  "\n", oldStackSize);
        OSReport("[MEM] newStackBase  : %p"      "\n", newStackBase);
        OSReport("[MEM] newStackSize  : 0x%08x"  "\n", newStackSize);
        OSReport("--------------------------------\n");
        // clang-format on
    }

    return REPLACED(
            OSCreateThread)(thread, func, param, newStackBase, newStackSize, priority, attr);
}
