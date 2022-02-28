#pragma once

//
// Super primitive slab allocator. The SO librarys normally slams the heap with frequent,
// small allocations of unique size.
//

#include <Common.h>

// Debug checks
#define SLAB_DEBUG

#define SLAB_SIZE(alloc_size, num) ((alloc_size + 4) * num)
#define NUM_SLABS(arr, alloc_size) (sizeof(arr) / (alloc_size + 4))

// The SO library does a bunch of small allocations
#define NET_SLABS_LIST(func) func(32, 8) func(64, 4) func(1024, 4)

typedef struct {
#define __NetSlabMember(granularity, count) \
    u8 slab_##granularity[SLAB_SIZE(granularity, count)];

    NET_SLABS_LIST(__NetSlabMember)
} NetSlabs;

extern NetSlabs *sSlabs;

void *TryAllocFromSlabs(int size);
bool TryFreeFromSlabs(void *ptr, int size);
