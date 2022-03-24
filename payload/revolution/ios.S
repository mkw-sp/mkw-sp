#include <Common.S>

.global real_IOS_Open
real_IOS_Open:
    stwu r1, -0x20(r1)
    mflr r0
    b IOS_Open+8

PATCH_B_START(IOS_Open, 4)
    addi r1, r1, 0x20 // Undo instruction before
    b my_IOS_Open
PATCH_B_END(IOS_Open, 4)

.global real_IOS_OpenAsync
real_IOS_OpenAsync:
    stwu r1, -0x30(r1)
    mflr r0
    b IOS_OpenAsync+8

PATCH_B_START(IOS_OpenAsync, 4)
    addi r1, r1, 0x30 // Undo instruction before
    b my_IOS_OpenAsync
PATCH_B_END(IOS_OpenAsync, 4)
