// Reference: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#once-ctor

#include <Common.h>
#include <revolution.h>

typedef struct {
    u8 is_init; /* This byte must be here, the rest are free */
    u8 _pad[3];

    /* For now, just disable multitasking */
    u32 msr_save;
} sp_guard;
static_assert(sizeof(sp_guard) == 8); /* As specified by ABI */

int __cxa_guard_acquire(sp_guard *guard) {
    guard->msr_save = OSDisableInterrupts();
    return 1;
}
void __cxa_guard_release(sp_guard *guard) {
    guard->is_init = 1;
    OSRestoreInterrupts(guard->msr_save);
}
