extern "C" {
#include "Net.h"
}

#include "sp/ScopeLock.hh"

#include <cstring>

namespace SP::Net {

static Mutex mutex;

static void *Alloc(u32 UNUSED(id), s32 size) {
    ScopeLock<Mutex> lock(mutex);

    void *slab_res = TryAllocFromSlabs(size);
    if (slab_res) {
        return slab_res;
    }

    assert(!"Bad alloc");
    return nullptr;
}

static void Free(u32 UNUSED(id), void *ptr, s32 size) {
    ScopeLock<Mutex> lock(mutex);

    // TODO: In theory, we do not need to lock a mutex to free from the slab allocator (?)
    if (TryFreeFromSlabs(ptr, size)) {
        return;
    }

    assert(!"Bad alloc");
}

static void Init() {
    sSlabs = reinterpret_cast<NetSlabs *>(OSAllocFromMEM2ArenaLo(sizeof(NetSlabs), 4));
    assert(sSlabs && "Failed to create slab allocator");
    memset(sSlabs, 0, sizeof(*sSlabs));

    SOLibraryConfig cfg;
    cfg.alloc = Alloc;
    cfg.free = Free;
    int res = SOInit(&cfg);
    SP_LOG("SOInit returned %i", res);
    assert(res == 0 && "SOInit failed");
}

} // namespace SP::Net

extern "C" void Net_Init() {
    return SP::Net::Init();
}
