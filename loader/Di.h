#pragma once

#include <Common.h>

bool Di_init(void);

bool Di_readDiskId(void);

bool Di_readUnencrypted(void *dst, u32 size, u32 offset);

bool Di_openPartition(u32 offset);

bool Di_read(void *dst, u32 size, u32 offset);

bool Di_isInserted(void);

bool Di_reset(void);

bool Di_deinit(void);
