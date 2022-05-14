#include <Common.S>

.global real_EGG_ExpHeap_alloc
real_EGG_ExpHeap_alloc:
    stwu r1, -0x20(r1)
    mflr r0
    b EGG_ExpHeap_alloc+8

PATCH_B_START(EGG_ExpHeap_alloc, 4)
    addi r1, r1, 0x20 // Undo instruction before
    mflr r6
    b my_EGG_ExpHeap_alloc
PATCH_B_END(EGG_ExpHeap_alloc, 4)

.global real_EGG_ExpHeap_free
real_EGG_ExpHeap_free:
    lwz r3, 0x10 (r3)
    b EGG_ExpHeap_free+4

PATCH_B_START(EGG_ExpHeap_free, 0)
    b my_EGG_ExpHeap_free
PATCH_B_END(EGG_ExpHeap_free, 0)
