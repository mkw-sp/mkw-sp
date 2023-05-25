#include <Common.S>

PATCH_BL_START(Snd_806fafb4, 0x44)
    extrwi r27, r0, 1, 29 // Original instruction

    // Check that 200cc is enabled
    lis r5, g_speedModIsEnabled@ha
    lbz r5, g_speedModIsEnabled@l (r5)
    cmpwi r5, 0x0
    beqlr

    // Check that the player is drifting
    clrlwi r5, r0, 27
    cmpwi r5, 0xf
    bnelr

    // Check that the brake drifting bit is set
    lis r5, s_raceManager@ha
    lwz r5, s_raceManager@l (r5)
    lwz r5, 0xc (r5)
    lwz r6, 0x18 (r4)
    lwz r6, 0x0 (r6)
    lwz r6, 0x0 (r6)
    lbz r6, 0x10 (r6)
    slwi r6, r6, 2
    lwzx r5, r5, r6
    lwz r5, 0x48 (r5)
    lhz r5, 0x2c (r5)
    extrwi. r5, r5, 1, 27
    beqlr

    extrwi. r5, r0, 1, 0
    bnelr

    extrwi. r5, r0, 1, 11
    bnelr

    li r27, 0x0
    li r28, 0x1
    li r30, 0x0
    blr
PATCH_BL_END(Snd_806fafb4, 0x44)



PATCH_BL_START(Snd_80716170, 0x10)
    lwz r3, 0x10 (r3) // Original instruction, get section id

    // Check that the section id is ghost replay
    cmpwi r3, 0x34
    bnelr

    // Enable the loading of character voices
    li r3, 0x3
    b Snd_80716170 + 0x18
PATCH_BL_END(Snd_80716170, 0x10)

PATCH_REPLACE_START(Snd_80716170, 0xa0)
    nop // Remove ghost player type check
PATCH_REPLACE_END(Snd_80716170, 0xa0)

PATCH_BL_START(Snd_80716170, 0xd0)
    cmpwi r3, 0x0 // Original instruction, check if the player is local
    bnelr // If that's the case we are done

    // Otherwise check if the player is a ghost
    mr r3, r28
    bl KartObjectProxy_isGhost

    cmpwi r3, 0x0 // Original instruction again
    b Snd_80716170 + 0xd4
PATCH_BL_END(Snd_80716170, 0xd0)



PATCH_REPLACE_START(Snd_80719044, 0x8f0)
    // Remove 1st lap check. This fixes TF with dynamic speedup.
    nop
PATCH_REPLACE_END(Snd_80719044, 0x8f0)
