#include "Patcher.h"

#include "../system/Memory.h"
#include "../../sp/Storage.h"

#include <revolution.h>

#include <Dol.h>
#include <Rel.h>

extern u32 payloadSize;

__attribute__((section("first"))) void start(void) {
    OSAllocFromMEM1ArenaLo(Rel_getSize(), 0x20);
    OSAllocFromMEM1ArenaLo(payloadSize, 0x20);

    Patcher_patch(PATCHER_BINARY_DOL);
    ProtectRangeModule(OS_PROTECT_CHANNEL_0, Dol_getInitSectionStart(), Dol_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    ProtectRangeModule(OS_PROTECT_CHANNEL_1, Dol_getSdata2SectionStart(), Dol_getSbss2SectionEnd(), OS_PROTECT_PERMISSION_READ);

    assert(Storage_init());
    DVDExInit();
}
