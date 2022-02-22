#pragma once

#include <Common.h>
#include <revolution/os/OSMemory.h>

// Rounding
#define MemoryRoundDown1024B(address) ((unsigned int)address & ~0x3FF)
#define MemoryRoundUp1024B(address) (((unsigned int)address + 0x3FF) & ~0x3FF)

void ProtectRangeModule(u32 channel, void* start, void* end, u32 permissions);
