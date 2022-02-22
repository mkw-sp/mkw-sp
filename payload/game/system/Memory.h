#pragma once

#include <revolution/os/OSMemory.h>

// Rounding
#define MemoryRoundDown1024B(address) ((unsigned int)address & ~0x3FF)
#define MemoryRoundUp1024B(address) (((unsigned int)address + 0x3FF) & ~0x3FF)

void ProtectRangeModule(unsigned int channel, void* start, void* end, unsigned int permissions);
