#include <Common.S>

#ifdef SP_CHANNEL
.section "bootstrap", "ax"
#endif
.global start
start:
#ifdef SP_CHANNEL
    lis r3, .Lstartup@h
    andis. r3, r3, 0x7fff
    ori r3, r3, .Lstartup@l
    mtspr SRR0, r3
    li r4, 0
    mtspr SRR1, r4
    rfi

    .balign 0x100
.Lstartup:
    lis r3, 0x0011
    ori r3, r3, 0x0c64
    mtspr HID0, r3
    isync

    mfspr r4, HID4
    rlwinm r4, r4, 0, 7, 5
    mtspr HID4, r4

    ori r4, r3, 0xc000
    mtspr HID0, r4
    isync

    // Reset DBATs
    li r4, 0
    mtspr DBAT0U, r4
    mtspr DBAT1U, r4
    mtspr DBAT2U, r4
    mtspr DBAT3U, r4
    mtspr DBAT4U, r4
    mtspr DBAT5U, r4
    mtspr DBAT6U, r4
    mtspr DBAT7U, r4
    isync

    // Reset segment registers
    lis r4, 0x8000
    ori r4, r4, 0x0000
    mtsr 0, r4
    mtsr 1, r4
    mtsr 2, r4
    mtsr 3, r4
    mtsr 4, r4
    mtsr 5, r4
    mtsr 6, r4
    mtsr 7, r4
    mtsr 8, r4
    mtsr 9, r4
    mtsr 10, r4
    mtsr 11, r4
    mtsr 12, r4
    mtsr 13, r4
    mtsr 14, r4
    mtsr 15, r4

    // Setup MEM1 BATs
    lis r4, 0x0000
    ori r4, r4, 0x0002
    lis r3, 0x8000
    ori r3, r3, 0x1fff
    mtspr DBAT0L, r4
    mtspr DBAT0U, r3
    isync
    mtspr IBAT0L, r4
    mtspr IBAT0U, r3
    isync

    lis r4, 0x0000
    ori r4, r4, 0x002a
    lis r3, 0xc000
    ori r3, r3, 0x1fff
    mtspr DBAT1L, r4
    mtspr DBAT1U, r3
    isync

    // Setup MEM2 BATs
    lis r4, 0x1000
    ori r4, r4, 0x0002
    lis r3, 0x9000
    ori r3, r3, 0x1fff
    mtspr DBAT4L, r4
    mtspr DBAT4U, r3
    isync
    mtspr IBAT4L, r4
    mtspr IBAT4U, r3
    isync

    lis r4, 0x1000
    ori r4, r4, 0x002a
    lis r3, 0xd000
    ori r3, r3, 0x1fff
    mtspr DBAT5L, r4
    mtspr DBAT5U, r3
    isync

    // Enable MEM2
    lis r3, 0x8200
    mtspr HID4, r3

    // Write default exception handlers (just a rfi)
    lis r3, 0x4c00
    ori r3, r3, 0x0064
    lis r4, 0x0000
    stw r3, 0x0100 (r4)
    stw r3, 0x0200 (r4)
    stw r3, 0x0300 (r4)
    stw r3, 0x0400 (r4)
    stw r3, 0x0500 (r4)
    stw r3, 0x0600 (r4)
    stw r3, 0x0700 (r4)
    stw r3, 0x0800 (r4)
    stw r3, 0x0900 (r4)
    stw r3, 0x0c00 (r4)
    stw r3, 0x0d00 (r4)
    stw r3, 0x0f00 (r4)
    stw r3, 0x1300 (r4)
    stw r3, 0x1400 (r4)
    stw r3, 0x1700 (r4)

    // Write the bus clock
    lis r3, 0x0e7b
    ori r3, r3, 0xe2c0
    stw r3, 0x00f8 (r4)

    // Write the CPU clock
    lis r3, 0x2b73
    ori r3, r3, 0xa840
    stw r3, 0x00fc (r4)

    // Initialize the stack pointer
    lis r1, stackTop@h
    ori r1, r1, stackTop@l

    // Fallback to an infinite loop if the C++ code fails
    lis r3, abort@h
    ori r3, r3, abort@l
    mtlr r3

    // Prepare to jump to the C++ code
    lis r3, Stub_Run@h
    ori r3, r3, Stub_Run@l
    mtspr SRR0, r3

    // Jump to it by returning from real mode
    mfmsr r4
    ori r4, r4, 0x30
    mtspr SRR1, r4
    rfi
#else
    // Disable external interrupts
    mfmsr r3
    rlwinm r3, r3, 0, 17, 15
    mtmsr r3

    // Initialize the stack pointer
    lis r1, stackTop@h
    ori r1, r1, stackTop@l

    // Jump to C++ code
    bl Stub_Run

    // We should never get there
    b 0x0
#endif
.size start, . - start

.global abort
abort:
    // Or there...
    b 0x0
.size abort, . - abort

    // Dummy: the presence of such an instruction is the criterion for Dolphin to load an ELF in Wii
    // mode instead of GC.
    mtspr HID4, r3
