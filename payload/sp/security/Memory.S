#include <Common.S>

RealModeTrampoline:
    mfmsr      r10
    rlwinm     r9, r10, 0, ~(MSR_EE)
    mtmsr      r9
    rlwinm     r9, r9, 0, ~(MSR_IR | MSR_DR)
    mtspr      SRR1, r9
    mflr       r9
    rlwinm     r9, r9, 0, 0x3FFFFFFF
    mtspr      SRR0, r9
    rfi

.globl SetSDR1
SetSDR1:
    mflr       r8
    bl         RealModeTrampoline

    mtspr      SRR0, r8
    mtspr      SRR1, r10
    sync
    mtsdr1     r3
    rfi

.globl InitSRs
InitSRs:
    li         r0, 0
    lis        r3, 0x8000

    mtsr       0, r3
    mtsr       1, r3
    mtsr       2, r3
    mtsr       3, r3
    mtsr       4, r3
    mtsr       5, r3
    mtsr       6, r3
    mtsr       7, r3
    mtsr       8, r0
    mtsr       9, r3
    mtsr      10, r3
    mtsr      11, r3
    mtsr      12, r3
    mtsr      13, r3
    mtsr      14, r3
    mtsr      15, r3
    isync
    blr

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
