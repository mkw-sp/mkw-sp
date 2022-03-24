#include "Stack.h"

#include <revolution/os.h>
#include <sp/Version.h>

u32 __stack_chk_guard;

void Stack_InitCanary(void)
{
    __stack_chk_guard = OSGetTick() & 0x00FFFFFF;
}

__attribute__((noreturn)) void __stack_chk_fail(void)
{
    const GXColor background = { 0xFF, 0x00, 0x00, 0xFF };
    const GXColor foreground = { 0xFF, 0xFF, 0xFF, 0xFF };

    OSFatal(foreground, background, "MKW-SP v" BUILD_TYPE_STR "\n\n" "Stack smashing detected !");
    __builtin_unreachable();
}
