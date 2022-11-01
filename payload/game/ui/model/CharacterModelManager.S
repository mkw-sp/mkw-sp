#include <Common.S>

// In case we want to expand this later
#define CHARA_COUNT 24

PATCH_B_START(CharacterModelManager_ct, 0x14)
    mulli   r0, r4, CHARA_COUNT
    add     r0, r5, r0
    b       CharacterModelManager_ct + 0x18
PATCH_B_END(CharacterModelManager_ct, 0x14)

PATCH_B_START(CharacterModelManager_ct, 0x19C)
    // i % CHARA_COUNT
    mulli   r0, r4, CHARA_COUNT
    li      r4, CHARA_COUNT
    divw    r4, r29, r4
    mulli   r4, r4, CHARA_COUNT
    sub     r4, r29, r4
    rlwinm  r24, r4, 0, 0xFF
    b       CharacterModelManager_ct + 0x1A0
PATCH_B_END(CharacterModelManager_ct, 0x19C)

PATCH_B_START(CharacterModelManager_ct, 0x268)
    lbz     r0, 8(r27)
    mulli   r0, r0, CHARA_COUNT
    cmpw    r29, r0
    b       CharacterModelManager_ct + 0x26C
PATCH_B_END(CharacterModelManager_ct, 0x268)

// The model index to display on the menu
PATCH_B_START(CharacterModelManager_80830d00, 0xD0)
    // To get the player ID back
    li      r12, 12
    divw    r0, r0, r12
    mulli   r0, r0, CHARA_COUNT
    add     r0, r5, r0
    mulli   r0, r0, 0x28
    b       CharacterModelManager_80830d00 + 0xD4
PATCH_B_END(CharacterModelManager_80830d00, 0xD0)

// Fix the Mii model index
PATCH_B_START(CharacterModelManager_80830d00, 0x218)
    lbz     r4, 8(r26) // Player count
    subi    r4, r4, 1
    mulli   r4, r4, CHARA_COUNT
    add     r8, r8, r4

    mulli   r28, r8, 0x28 // Original instruction
    b       CharacterModelManager_80830d00 + 0x21C
PATCH_B_END(CharacterModelManager_80830d00, 0x218)

PATCH_B_START(CharacterModelManager_8083104c, 0x3C)
    lbz     r5, 8(r3) // Player count
    subi    r5, r5, 1
    mulli   r5, r5, CHARA_COUNT
    add     r30, r30, r5

    addi    r0, r30, CHARA_COUNT // Original instruction
    b       CharacterModelManager_8083104c + 0x40
PATCH_B_END(CharacterModelManager_8083104c, 0x3C)

PATCH_B_START(CharacterModelManager_80831534, 0x90)
    lbz     r5, 8(r3) // Player count
    subi    r5, r5, 1
    mulli   r5, r5, CHARA_COUNT
    add     r26, r26, r5

    addi    r0, r26, CHARA_COUNT // Original instruction
    b       CharacterModelManager_80831534 + 0x94
PATCH_B_END(CharacterModelManager_80831534, 0x90)

PATCH_B_START(CharacterModelManager_80831100, 0x50)
    lbz     r30, 8(r25) // Player count
    mulli   r30, r30, CHARA_COUNT
    add     r30, r30, r4

    b       CharacterModelManager_80831100 + 0x54
PATCH_B_END(CharacterModelManager_80831100, 0x50)

// Toadette pigtails are annoying

PATCH_B_START(CharacterModelManager_ct, 0x228)
    // Check if pigtails have been allocated yet
    lwz     r3, 0x4C(r27)
    cmpwi   r3, 0
    bne-    alreadyAllocated

    // Allocate new pigtails
    lbz     r3, 8(r27)
    mulli   r3, r3, 0x58
    bl      new
    stw     r3, 0x4C(r27)
    b       alreadyAllocated

alreadyAllocated:
    // To get player index
    li      r4, CHARA_COUNT
    divw    r4, r29, r4

    // Get index in pigtails
    mulli   r4, r4, 0x58
    add     r3, r3, r4
    b       CharacterModelManager_ct + 0x22C
PATCH_B_END(CharacterModelManager_ct, 0x228)

PATCH_REPLACE_START(CharacterModelManager_ct, 0x254)
    // Don't set pigtails ptr
    nop
PATCH_REPLACE_END(CharacterModelManager_ct, 0x254)

PATCH_B_START(CharacterModelManager_ct, 0x2D8)
    lbz     r28, 8(r27) // Local player count
    mulli   r28, r28, CHARA_COUNT * 0x28
    b       CharacterModelManager_ct + 0x2DC
PATCH_B_END(CharacterModelManager_ct, 0x2D8)

PATCH_B_START(CharacterModelManager_80830a80, 0x68)
    bl      SetModelVisible // First call

    lbz     r29, 0x8(r30) // Local player count
    cmpwi   r29, 2
    blt-    pigtailsFix01_end

    lwz     r3, 0x4C(r30)
    addi    r3, r3, 0x58
    li      r4, 0
    bl      SetModelVisible

    cmpwi   r29, 3
    blt-    pigtailsFix01_end

    lwz     r3, 0x4C(r30)
    addi    r3, r3, 0x58 * 2
    li      r4, 0
    bl      SetModelVisible

    cmpwi   r29, 4
    blt-    pigtailsFix01_end

    lwz     r3, 0x4C(r30)
    addi    r3, r3, 0x58 * 3
    li      r4, 0
    bl      SetModelVisible

pigtailsFix01_end:
    b       CharacterModelManager_80830a80 + 0x6C
PATCH_B_END(CharacterModelManager_80830a80, 0x68)

PATCH_B_START(CharacterModelManager_80830a80, 0x168)
    // The parameter gets discarded but we can get it back!
    sub     r5, r31, r30
    li      r6, 0xC
    divw    r5, r5, r6

    mulli   r5, r5, (0x28 * CHARA_COUNT)
    addi    r5, r5, 4 + (0x28 * 13) // Toadette index
    lwzx    r0, r3, r5
    b       CharacterModelManager_80830a80 + 0x16C
PATCH_B_END(CharacterModelManager_80830a80, 0x168)

PATCH_B_START(CharacterModelManager_80830a80, 0x178)
    cmpwi   r3, 0
    beq     pigtailsFix03_end

    // The parameter gets discarded but we can get it back!
    sub     r4, r31, r30
    li      r5, 0xC
    divw    r4, r4, r5

    mulli   r4, r4, 0x58 // Local player ID
    add     r3, r3, r4
pigtailsFix03_end:
    b       CharacterModelManager_80830a80 + 0x17C
PATCH_B_END(CharacterModelManager_80830a80, 0x178)

PATCH_B_START(CharacterModelManager_808307b0, 0x84)
    li      r28, 0
    mr      r29, r3

    // Original instruction
    bctrl

    // Now loop the same call for each player
pigtailsFix04_loop:

    addi    r28, r28, 1
    lbz     r0, 8(r30) // Player count
    cmpw    r28, r0
    bge     pigtailsFix04_end

    addi    r29, r29, 0x58
    mr      r3, r29
    li      r4, 0

    lwz     r12, 0(r3)
    lwz     r12, 0xC(r12)
    mtctr   r12
    bctrl
    b       pigtailsFix04_loop

pigtailsFix04_end:
    b       CharacterModelManager_808307b0 + 0x88
PATCH_B_END(CharacterModelManager_808307b0, 0x84)

PATCH_B_START(CharacterModelManager_808307b0, 0x1A0)
    mulli   r12, r31, (0x28 * CHARA_COUNT)
    addi    r12, r12, 4 + (0x28 * 13) // Toadette index
    lwzx    r0, r3, r12
    b       CharacterModelManager_808307b0 + 0x1A4
PATCH_B_END(CharacterModelManager_808307b0, 0x1A0)

PATCH_B_START(CharacterModelManager_808307b0, 0x1B0)
    cmpwi   r3, 0
    beq     pigtailsFix05_end

    mulli   r4, r31, 0x58 // Local player ID
    add     r3, r3, r4
pigtailsFix05_end:
    b       CharacterModelManager_808307b0 + 0x1B4
PATCH_B_END(CharacterModelManager_808307b0, 0x1B0)

PATCH_B_START(CharacterModelManager_808307b0, 0x22C)
    lwz     r3, 0x4C(r30)
    mulli   r4, r31, 0x58 // Local player ID
    add     r3, r3, r4
    b       CharacterModelManager_808307b0 + 0x230
PATCH_B_END(CharacterModelManager_808307b0, 0x22C)

PATCH_B_START(CharacterModelManager_808307b0, 0x23C)
    lwz     r3, 0x4C(r30)
    mulli   r4, r31, 0x58 // Local player ID
    add     r3, r3, r4
    b       CharacterModelManager_808307b0 + 0x240
PATCH_B_END(CharacterModelManager_808307b0, 0x23C)
