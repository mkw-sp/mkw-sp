#include <Common.S>

.globl InvalidateAllIBATs
InvalidateAllIBATs:
    li        r0, 0

    mtspr     IBAT0U, r0
    mtspr     IBAT0L, r0
    mtspr     IBAT1U, r0
    mtspr     IBAT1L, r0
    mtspr     IBAT2U, r0
    mtspr     IBAT2L, r0
    mtspr     IBAT3U, r0
    mtspr     IBAT3L, r0
    mtspr     IBAT4U, r0
    mtspr     IBAT4L, r0
    mtspr     IBAT5U, r0
    mtspr     IBAT5L, r0
    mtspr     IBAT6U, r0
    mtspr     IBAT6L, r0
    mtspr     IBAT7U, r0
    mtspr     IBAT7L, r0
    isync

    mfmsr     r3
    ori       r3, r3, MSR_DR | MSR_IR
    mtspr     SRR1, r3
    mfspr     r3, LR
    mtspr     SRR0, r3
    rfi
