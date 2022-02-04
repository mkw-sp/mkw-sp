#include <Common.S>

PATCH_BL_START(MainLayout_load, 0x30)
    mr r6, r5
    mr r5, r4
    li r4, 0x40
    b snprintf
PATCH_BL_END(MainLayout_load, 0x30)

PATCH_BL_START(PictureSourceLayoutList_load, 0x294)
    mr r6, r5
    mr r5, r4
    li r4, 0x40
    b snprintf
PATCH_BL_END(PictureSourceLayoutList_load, 0x294)
