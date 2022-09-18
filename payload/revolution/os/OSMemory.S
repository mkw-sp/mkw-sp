#include <Common.S>
#include <revolution/os.h>

# Invalidate IBAT4
PATCH_REPLACE_START(ConfigMEM2_56MB, 0x3C)
    mtspr IBAT4L, r7
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0x3C)

# Invalidate IBAT6
PATCH_REPLACE_START(ConfigMEM2_56MB, 0x9C)
    mtspr IBAT6L, r7
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0x9C)

# Invalidate IBAT7
PATCH_REPLACE_START(ConfigMEM2_56MB, 0xBC)
    mtspr IBAT7L, r7
    nop
PATCH_REPLACE_END(ConfigMEM2_56MB, 0xBC)

.globl EnableInstsOnExecSections
EnableInstsOnExecSections:
    li        r0, 0

    lis       r3, newIBAT0U@ha
    lis       r4, newIBAT1U@ha
    lis       r5, newIBAT2U@ha

    rlwinm    r3, r3, 0, 3, 15
    rlwinm    r4, r4, 0, 3, 15
    rlwinm    r5, r5, 0, 3, 15

    lwz       r3, newIBAT0U@l(r3)
    lwz       r4, newIBAT1U@l(r4)
    lwz       r5, newIBAT2U@l(r5)

    li        r6, BAT_PP_RW

    rlwinm    r7, r4, 0, 3, 14
    ori       r7, r7, BAT_PP_RW
    rlwinm    r8, r5, 0, 3, 14
    ori       r8, r8, BAT_PP_RW

    # Dol
    mtspr     IBAT0U, r0
    mtspr     IBAT0L, r6
    mtspr     IBAT0U, r3

    # Rel
    mtspr     IBAT1U, r0
    mtspr     IBAT1L, r7
    mtspr     IBAT1U, r4

    # Payload
    mtspr     IBAT2U, r0
    mtspr     IBAT2L, r8
    mtspr     IBAT2U, r5
    isync

    mfmsr     r3
    ori       r3, r3, MSR_DR | MSR_IR
    mtspr     SRR1, r3
    mfspr     r3, LR
    mtspr     SRR0, r3
    rfi
