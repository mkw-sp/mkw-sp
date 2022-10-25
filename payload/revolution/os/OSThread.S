#include <Common.S>

PATCH_REPLACE_START(__OSThreadInit, 0x7C)
    lis       r4, main_thread_stack_pointer_top@ha
    lis       r5, main_thread_stack_pointer_bottom@ha
    lwz       r4, main_thread_stack_pointer_top@l(r4)
    lis       r3, 0xDEAE
    lwz       r5, main_thread_stack_pointer_bottom@l(r5)
PATCH_REPLACE_END(__OSThreadInit, 0x7C)
