#pragma once

#include <Common.h>

void DCInvalidateRange(void *start, u32 size);

void DCFlushRange(void *start, u32 size);

void DCStoreRange(void *start, u32 size);

void ICInvalidateRange(void *start, u32 size);
