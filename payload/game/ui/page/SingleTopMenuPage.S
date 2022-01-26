#include <Common.S>

// When coming from time trials the second button is selected but since we don't have grand prix
// this behavior becomes incorrect, so remove it.
PATCH_REPLACE_START(SingleTopMenuPage_onInit, 0x50)
    b 0x38
PATCH_REPLACE_END(SingleTopMenuPage_onInit, 0x50)

PATCH_B_START(SingleTopMenuPage_vf_64, 0x80)
    addi r5, r5, 1
    slwi r0, r5, 2 // Original instruction

    b SingleTopMenuPage_vf_64 + 0x84
PATCH_B_END(SingleTopMenuPage_vf_64, 0x80)

PATCH_B_START(SingleTopMenuPage_vf_64, 0xc0)
    lwz r5, 0x240 (r28) // Original instruction
    addi r5, r5, 1

    b SingleTopMenuPage_vf_64 + 0xc4
PATCH_B_END(SingleTopMenuPage_vf_64, 0xc0)

PATCH_B_START(SingleTopMenuPage_vf_64, 0x16c)
    lwz r5, 0x240 (r28) // Original instruction
    addi r5, r5, 1

    b SingleTopMenuPage_vf_64 + 0x170
PATCH_B_END(SingleTopMenuPage_vf_64, 0x16c)

PATCH_B_START(SingleTopMenuPage_vf_64, 0x21c)
    lwz r5, 0x240 (r28) // Original instruction
    addi r5, r5, 1

    b SingleTopMenuPage_vf_64 + 0x220
PATCH_B_END(SingleTopMenuPage_vf_64, 0x21c)

PATCH_B_START(SingleTopMenuPage_vf_64, 0x2cc)
    lwz r5, 0x240 (r28) // Original instruction
    addi r5, r5, 1

    b SingleTopMenuPage_vf_64 + 0x2d0
PATCH_B_END(SingleTopMenuPage_vf_64, 0x2cc)

PATCH_B_START(SingleTopMenuPage_vf_64, 0x400)
    lwz r3, 0x240 (r28)
    addi r0, r3, 1

    b SingleTopMenuPage_vf_64 + 0x404
PATCH_B_END(SingleTopMenuPage_vf_64, 0x400)

PATCH_REPLACE_START(SingleTopMenuPage_vf_84, 0x1c)
    addi r28, r4, 1
PATCH_REPLACE_END(SingleTopMenuPage_vf_84, 0x1c)

PATCH_B_START(SingleTopMenuPage_onButtonFront, 0xe8)
    lwz r4, 0x240 (r4) // Original instruction
    addi r4, r4, 1

    b SingleTopMenuPage_onButtonFront + 0xec
PATCH_B_END(SingleTopMenuPage_onButtonFront, 0xe8)

PATCH_REPLACE_START(SingleTopMenuPage_onButtonFront, 0x20c)
    // Adjust the check for the back button id
    cmpwi r4, -100 + 1
PATCH_REPLACE_END(SingleTopMenuPage_onButtonFront, 0x20c)

// Load the rules page that replaces the gp class selection when the
// time trials button is pressed.
PATCH_REPLACE_START(SingleTopMenuPage_onButtonFront, 0xd4)
    li r27, 0x6a
PATCH_REPLACE_END(SingleTopMenuPage_onButtonFront, 0xd4)
