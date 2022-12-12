#pragma once

#include <Common.h>

void Memory_InvalidateAllIBATs(void);

void Memory_ProtectRange(u32 channel, void *start, void *end, u32 permissions);
