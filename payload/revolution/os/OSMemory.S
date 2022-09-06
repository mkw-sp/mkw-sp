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

.globl DisableInstsOnMEM1Hi16MB
DisableInstsOnMEM1Hi16MB:
    li        r0, 0
    li        r3, BAT_PP_RW
    lis       r4, OS_CACHED_BASE >> 16
    ori       r4, r4, BAT_BL_8MB | BAT_VS | BAT_VP

    mtspr     IBAT0U, r0
    mtspr     IBAT0L, r3
    mtspr     IBAT0U, r4
    mtspr     IBAT2U, r0
    mtspr     IBAT2L, r0
    isync

    mfmsr     r3
    ori       r3, r3, MSR_DR | MSR_IR
    mtspr     SRR1, r3
    mfspr     r3, LR
    mtspr     SRR0, r3
    rfi

.globl EnableInstsOnPayload
EnableInstsOnPayload:
    li        r0, 0
    li        r3, BAT_PP_RW
    lis       r4, newIBAT1U@ha
    rlwinm    r4, r4, 0, 3, 15
    lwz       r4, newIBAT1U@l(r4)

    mtspr     IBAT1U, r0
    mtspr     IBAT1L, r3
    mtspr     IBAT1U, r4
    isync

    mfmsr     r3
    ori       r3, r3, MSR_DR | MSR_IR
    mtspr     SRR1, r3
    mfspr     r3, LR
    mtspr     SRR0, r3
    rfi
