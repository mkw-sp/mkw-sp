#include <Common.S>

.global fix_RacePage_initInputs
fix_RacePage_initInputs:
    # Check if the section id is ghost replay
    cmpwi r0, 0x34
    bne .LinitInputs_end

    li r0, 0x6a # Pretend it's live view instead

.LinitInputs_end:
    cmpwi r0, 0x6a # Original instruction
    blr
.size fix_RacePage_initInputs, . - fix_RacePage_initInputs

PATCH_BL(RacePage_initInputs + 0x240, fix_RacePage_initInputs)



PATCH_INJECT_START(RacePage_getControlCount, 0x198)
    add r3, r3, r30 # Increase the control count by one CtrlRaceSpeed per local player

    lwz r31, 0x1c (r1) # Original instruction
PATCH_INJECT_END(RacePage_getControlCount, 0x198)



PATCH_INJECT_START(RacePage_getControlCount, 0xc4)
    cmpwi r3, 0 # Check if the number of CtrlRaceNameBalloon is zero
    beq .LgetControlCount_end

    li r3, 12 # If not, set it to 12

.LgetControlCount_end:
    add r31, r31, r3 # Original instruction
PATCH_INJECT_END(RacePage_getControlCount, 0xc4)



PATCH_INJECT_START(RacePage_initControls, 0x3e0)
    mr r3, r15 # this
    mr r4, r18 # controlId
    mr r5, r19 # localPlayerCount
    mr r6, r17 # localPlayerId
    bl RacePage_initSpeedControl
    
    addi r18, r18, 1 # Increment the control id

    addi r17, r17, 1 # Original instruction, increment the local player id
PATCH_INJECT_END(RacePage_initControls, 0x3e0)



# If the number of CtrlRaceNameBalloon is not zero, set it to 12

PATCH_REPLACE_START(RacePage_initControls, 0x5cc)
    li r3, 12
PATCH_REPLACE_END(RacePage_initControls, 0x5cc)

PATCH_REPLACE_START(RacePage_initControls, 0x680)
    li r3, 12
PATCH_REPLACE_END(RacePage_initControls, 0x680)



.global fix_RacePage_beforeCalc
fix_RacePage_beforeCalc:
    # Check if the section id is live replay
    cmpwi r0, 0x34
    bne .LbeforeCalc_end

    # Check that the countdown has started
    lis r4, s_raceManager@ha
    lwz r4, s_raceManager@l (r4)
    lwz r4, 0x28 (r4)
    cmpwi r4, 0x0
    beq .LbeforeCalc_end

    li r0, 0x6a # If the section is ghost replay, pretend it's live view instead

.LbeforeCalc_end:
    cmpwi r0, 0x6a # Original instruction
    blr
.size fix_RacePage_beforeCalc, . - fix_RacePage_beforeCalc

PATCH_BL(RacePage_beforeCalc + 0x3c, fix_RacePage_beforeCalc)



# Wait until the end of the race in ghost replay

PATCH_REPLACE_START(RacePage_canChangeSection, 0x14)
    nop
PATCH_REPLACE_END(RacePage_canChangeSection, 0x14)

PATCH_REPLACE_START(RacePage_canReplace, 0xd4)
    b 0x1c
PATCH_REPLACE_END(RacePage_canReplace, 0xd4)
