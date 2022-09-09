#include <Common.S>

#define MAIL_KWB_FILE_SIZE 0x1E0

PATCH_BL_START(NWC24_8066DAD0, 0x50)
    cmpwi     r3, 0
    beqlr-

    lwz       r0, 0x20(r1)
    cmpwi     r0, MAIL_KWB_FILE_SIZE
    crnot     4*cr0+eq, 4*cr0+eq
    blr
PATCH_BL_END(NWC24_8066DAD0, 0x50)
