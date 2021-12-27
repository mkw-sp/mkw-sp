#pragma once

#include <Common.h>

void DCFlushRange(void *addr, u32 nBytes);

void ICInvalidateRange(void *addr, u32 nBytes);
