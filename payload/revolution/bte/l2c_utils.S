#include <Common.S>

#define L2CAP_BASE_APPL_CID 0x40

#define MAX_L2CAP_CHANNELS  10

PATCH_REPLACE_START(l2cu_find_ccb_by_cid, 0x0)
    addi      r0, r4, -L2CAP_BASE_APPL_CID
    cmplwi    r0, MAX_L2CAP_CHANNELS - 1
    li        r5, 0
    bgt-      0x48
PATCH_REPLACE_END(l2cu_find_ccb_by_cid, 0x0)
