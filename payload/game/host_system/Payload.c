#include "Patcher.h"

#include "../system/Memory.h"

#include <sp/Host.h>
#include <sp/LogFile.h>
#include <sp/Net.h>
#include <sp/Storage.h>

#include <revolution.h>

#include <string.h>

#include <Dol.h>
#include <Payload.h>
#include <Rel.h>

extern u32 payloadSize;

__attribute__((section("first"))) void start(void) {
    ProtectRangeModule(OS_PROTECT_CHANNEL_0, Payload_getTextSectionStart(), Payload_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);

    OSAllocFromMEM1ArenaLo(Rel_getSize(), 0x20);
    OSAllocFromMEM1ArenaLo(payloadSize, 0x20);

    // We don't clear the arena in OSInit because the payload is already copied at that point, but
    // some code expects it to be zeroed.
    memset(OSGetMEM1ArenaLo(), 0, OSGetMEM1ArenaHi() - OSGetMEM1ArenaLo());

    Patcher_patch(PATCHER_BINARY_DOL);
    ProtectRangeModule(OS_PROTECT_CHANNEL_1, Dol_getInitSectionStart(), Dol_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    ProtectRangeModule(OS_PROTECT_CHANNEL_2, Dol_getSdata2SectionStart(), Dol_getSbss2SectionEnd(), OS_PROTECT_PERMISSION_READ);

    Host_Init();

    bool netWasInit = Net_initFromArena();
    assert(netWasInit);

    bool storageWasInit = Storage_init();
    assert(storageWasInit);

    LogFile_init();

    // Example output:
    //     --------------------------------
    //     MKW-SP v0.1.4 (Release) REV 15610c0
    //     Region: PAL, System: Dolphin 5.0-15993
    //     Built Mar  2 2022 at 23:22:40, GCC 10.2.0
    //     Changed files: 'configure.py, Payload.c, dbException.c, Host.c, Host.h, '
    //     --------------------------------
    Host_PrintMkwSpInfo(OSReport);

    DVDExInit();
}
