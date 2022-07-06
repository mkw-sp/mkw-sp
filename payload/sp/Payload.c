#include "Payload.h"

#include "sp/Channel.h"
#include "sp/Dol.h"
#include "sp/DVDDecompLoader.h"
#include "sp/Host.h"
#include "sp/Net.h"
#include "sp/Patcher.h"
#include "sp/Rel.h"
#include "sp/keyboard/SIKeyboard.h"
#include "sp/security/Memory.h"
#include "sp/security/Stack.h"
#include "sp/storage/LogFile.h"
#include "sp/storage/Storage.h"
#include "sp/storage/Usb.h"

#include <revolution.h>

#include <string.h>

extern void (*payload_ctors_start)(void);
extern void (*payload_ctors_end)(void);

void Payload_init(void) {
#ifndef GDB_COMPATIBLE
    OSDisableCodeExecOnMEM1Hi16MB();
#endif
    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_0, Payload_getTextSectionStart(), Payload_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);

#ifdef GDB_COMPATIBLE
    OSSetMEM1ArenaLo((void*)0x809C4FA0);
#else
    OSAllocFromMEM1ArenaLo(Rel_getSize(), 0x20);
#endif

    OSAllocFromMEM1ArenaLo(Payload_getSize(), 0x20);
    VIInit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    // We don't clear the arena in OSInit because the payload is already copied at that point, and
    // the XFB would turn green, but some code expects it to be zeroed.
    memset(OSGetMEM1ArenaLo(), 0, OSGetMEM1ArenaHi() - OSGetMEM1ArenaLo());

    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_1, Dol_getInitSectionStart(), Dol_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_2, Dol_getSdata2SectionStart(), Dol_getSbss2SectionEnd(), OS_PROTECT_PERMISSION_READ);

    Host_Init();

    bool usbWasInit = Usb_init();
    assert(usbWasInit);

    Net_Init();

    bool storageWasInit = Storage_init();
    assert(storageWasInit);

    LogFile_init();

    // Example output:
    //     --------------------------------
    //     MKW-SP v0.1.4 (Release) REV 15610c0
    //     Region: PAL, System: Dolphin 5.0-15993
    //     Built Mar  2 2022 at 23:22:40, GCC 10.2.0
    //     --------------------------------
    Host_PrintMkwSpInfo(OSReport);

    DVDExInit();

    DVDDecompLoader_init();

    SIKeyboard_InitSimple();

    Channel_Init();
}

__attribute__((no_stack_protector)) __attribute__((section("first"))) void Payload_run(void) {
    for (void (**ctor)(void) = &payload_ctors_start; ctor < &payload_ctors_end; ctor++) {
        (*ctor)();
    }
    Stack_InitCanary();
    Stack_RelocateMainThreadStackToMEM1End();
#ifndef GDB_COMPATIBLE
    Stack_DoLinkRegisterPatches(Dol_getTextSectionStart(), Dol_getTextSectionEnd());
#endif
    Patcher_patch(PATCHER_BINARY_DOL);
}
