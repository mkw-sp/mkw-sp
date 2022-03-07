#pragma once

#include <Common.h>
#include <sp/Slab.h>

enum {
    NET_HEAP_SIZE = sizeof(NetSlabs) + 5000,
};

bool Net_init(void);
