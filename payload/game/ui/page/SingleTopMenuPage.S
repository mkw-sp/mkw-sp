#include <Common.S>

// When coming from time trials the second button is selected but since we don't have grand prix
// this behavior becomes incorrect, so remove it.
PATCH_REPLACE_START(SingleTopMenuPage_onInit, 0x50)
    b 0x38
PATCH_REPLACE_END(SingleTopMenuPage_onInit, 0x50)

PATCH_INJECT_START(SingleTopMenuPage_vf_64, 0x80)
    addi r5, r5, 1
    slwi r0, r5, 2 # Original instruction
PATCH_INJECT_END(SingleTopMenuPage_vf_64, 0x80)

PATCH_INJECT_START(SingleTopMenuPage_vf_64, 0xc0)
    lwz r5, 0x240 (r28) # Original instruction
    addi r5, r5, 1
PATCH_INJECT_END(SingleTopMenuPage_vf_64, 0xc0)

PATCH_INJECT_START(SingleTopMenuPage_vf_64, 0x16c)
    lwz r5, 0x240 (r28) # Original instruction
    addi r5, r5, 1
PATCH_INJECT_END(SingleTopMenuPage_vf_64, 0x16c)

PATCH_INJECT_START(SingleTopMenuPage_vf_64, 0x21c)
    lwz r5, 0x240 (r28) # Original instruction
    addi r5, r5, 1
PATCH_INJECT_END(SingleTopMenuPage_vf_64, 0x21c)

PATCH_INJECT_START(SingleTopMenuPage_vf_64, 0x2cc)
    lwz r5, 0x240 (r28) # Original instruction
    addi r5, r5, 1
PATCH_INJECT_END(SingleTopMenuPage_vf_64, 0x2cc)

PATCH_INJECT_START(SingleTopMenuPage_vf_64, 0x400)
    lwz r3, 0x240 (r28)
    addi r0, r3, 1
PATCH_INJECT_END(SingleTopMenuPage_vf_64, 0x400)

PATCH_REPLACE_START(SingleTopMenuPage_vf_84, 0x1c)
    addi r28, r4, 1
PATCH_REPLACE_END(SingleTopMenuPage_vf_84, 0x1c)

PATCH_INJECT_START(SingleTopMenuPage_onButtonFront, 0xe8)
    lwz r4, 0x240 (r4) # Original instruction
    addi r4, r4, 1
PATCH_INJECT_END(SingleTopMenuPage_onButtonFront, 0xe8)
