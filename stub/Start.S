#include <Common.S>

.global start
start:
    // Disable external interrupts
    mfmsr r3
    rlwinm r3, r3, 0, 17, 15
    mtmsr r3

    // Initialize the stack pointer
    lis r1, stackTop@h
    ori r1, r1, stackTop@l

    // Jump to C++ code
    bl Stub_run

    // We should never get there
    b 0x0
.size start, . - start

    // Dummy: the presence of such an instruction is the criterion for Dolphin to load an ELF in Wii
    // mode instead of GC.
    mtspr HID4, r3
