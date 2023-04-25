#include "StackCanary.h"

#include <Common.h>

u32 __stack_chk_guard;

static void SetIABRRegister(u32 value) {
    asm volatile("mtspr 1010, %0" : "=r"(value));
}

void StackCanary_Init(void) {
    const u32 IABR_BE = 1 << 1;

    __stack_chk_guard = ((__builtin_ppc_mftb() & 0x00FFFFFF) | (0x80 << 24)) & ~IABR_BE;
    SetIABRRegister(__stack_chk_guard);
}

__attribute__((noreturn)) void __stack_chk_fail(void) {
    panic("Stack smashing detected!");
}
