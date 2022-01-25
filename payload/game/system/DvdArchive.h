#pragma once

#include <Common.h>

enum {
    DVD_ARCHIVE_STATE_MOUNTED = 0x4,
};

typedef struct {
    u8 _00[0x20 - 0x00];
    u32 state;
} DvdArchive;
static_assert(sizeof(DvdArchive) == 0x24);
