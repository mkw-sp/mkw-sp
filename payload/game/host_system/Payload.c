#include "Patcher.h"

#include <Dol.h>
#include <game/system/Memory.h>

__attribute__((section("first"))) void start(void) {
    Patcher_patch(PATCHER_BINARY_DOL);
    ProtectRangeModule(OS_PROTECT_CHANNEL_0, Dol_getInitSectionStart(), Dol_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    ProtectRangeModule(OS_PROTECT_CHANNEL_1, Dol_getSdata2SectionStart(), Dol_getSbss2SectionEnd(), OS_PROTECT_PERMISSION_READ);
}
