#include <Common.S>

PATCH_REPLACE_START(__init_registers, 0x74)
    lis       r1, main_thread_stack_pointer_top@ha
    lwz       r1, main_thread_stack_pointer_top@l(r1)
PATCH_REPLACE_END(__init_registers, 0x74)
