#include <Common.S>

.global thunk_RaceManagerPlayer_setMultiGhostPadProxy
thunk_RaceManagerPlayer_setMultiGhostPadProxy:
    mflr r29

    mr r3, r30
    bl RaceManagerPlayer_setMultiGhostPadProxy

    mr r3, r30 # Original instruction

    mtlr r29
    blr
.size thunk_RaceManagerPlayer_setMultiGhostPadProxy, . - thunk_RaceManagerPlayer_setMultiGhostPadProxy

PATCH_BL(RaceManagerPlayer_ct + 0x298, thunk_RaceManagerPlayer_setMultiGhostPadProxy)
