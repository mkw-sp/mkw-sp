#include "Slab.h"
#include <revolution.h>

static void *AllocSlab(u8 *slabs, int alloc_size, int num_slabs) {
    // TODO: Some bitmask optimization
    for (int i = 0; i < num_slabs; ++i) {
        u8 *slab = &slabs[SLAB_SIZE(alloc_size, i)];
        if (slab[0] == 0) {
            // OSReport("Slab alloc %i\n", i);
            slab[0] = 1;
            return slab + 32;
        }
    }

    return NULL;
}

#ifdef SLAB_DEBUG
static void FreeSlab(u8 *slabs, int alloc_size, int num_slabs, u8 *addr) {
    assert(addr <= slabs + SLAB_SIZE(alloc_size, num_slabs) && "Address is above slab structure");
    assert(addr >= slabs && "Address is below slab structure");
    assert(((addr - slabs - 32) % (alloc_size + 32)) == 0 && "Misaligned free");

    addr[-32] = 0;
}
#endif

NetSlabs *sSlabs;

void *TryAllocFromSlabs(int size) {
#define __NetSlabAlloc(granularity, count) \
    if (size <= granularity) { \
        return AllocSlab(sSlabs->slab_##granularity, granularity, count); \
    }

    NET_SLABS_LIST(__NetSlabAlloc);

    return NULL;
}

bool TryFreeFromSlabs(void *ptr, int size) {
    if ((u8 *)ptr < (u8 *)sSlabs) {
        return false;
    }
    if ((u8 *)ptr > (u8 *)(sSlabs + 1)) {
        return false;
    }

    // Catches if the pointer is in the wrong slab or misaligned
#ifdef SLAB_DEBUG
#define __NetSlabFree(granularity, count) \
    if (size <= granularity) { \
        FreeSlab(sSlabs->slab_##granularity, granularity, count, ptr); \
        return true; \
    }
    NET_SLABS_LIST(__NetSlabFree);

    assert(!"Invalid allocation");
#else
    (void)size;
    ((u8 *)ptr)[-32] = 0;
#endif

    return true;
}
