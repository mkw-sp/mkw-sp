#include <Common.S>

.section first
.global start
start:
    // Initialize the stack pointer
    lis r1, stackTop@h
    ori r1, r1, stackTop@l

    // Jump to C++ code
    bl Loader_run

    // We should never get there
    b 0x0
.size start, . - start
