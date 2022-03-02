#include "Patcher.h"

#include "../system/Memory.h"
#include "../../sp/LogFile.h"
#include "../../sp/Storage.h"
#include <sp/Net.h>

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

    // Start net
    bool netWasInit = Net_initFromArena();
    assert(netWasInit);

    bool storageWasInit = Storage_init();
    assert(storageWasInit);

    LogFile_init();

    DVDExInit();
}
