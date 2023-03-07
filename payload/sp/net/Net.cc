#include "Net.hh"

#include "sp/ScopeLock.hh"

#include <climits>
#include <cstring>

namespace SP::Net {

static Mutex mutex;
static OSThreadQueue queue;
static u8 stack[0x1000 /* 4 KiB */];
static OSThread thread;
static int res = INT_MIN;

void *Alloc(s32 size) {
    ScopeLock<Mutex> lock(mutex);

    void *slab_res = TryAllocFromSlabs(size);
    if (slab_res) {
        return slab_res;
    }

    assert(!"Bad alloc");
    return nullptr;
}

static void *Alloc(u32 /* id */, s32 size) {
    return Alloc(size);
}

void Free(void *ptr, s32 size) {
    ScopeLock<Mutex> lock(mutex);

    // TODO: In theory, we do not need to lock a mutex to free from the slab allocator (?)
    if (TryFreeFromSlabs(ptr, size)) {
        return;
    }

    assert(!"Bad alloc");
}

static void Free(u32 /* id */, void *ptr, s32 size) {
    return Free(ptr, size);
}

static void *Handle(void */* arg */) {
    res = SOStartup();
    while (true) {
        if (res != 0) {
            SOCleanup();
            res = SOStartup();
        } else {
            OSSleepThread(&queue);
        }
    }
}

void Init() {
    sSlabs = reinterpret_cast<NetSlabs *>(OSAllocFromMEM2ArenaLo(sizeof(NetSlabs), 32));
    assert(sSlabs && "Failed to create slab allocator");
    memset(sSlabs, 0, sizeof(*sSlabs));

    SOLibraryConfig cfg;
    cfg.alloc = Alloc;
    cfg.free = Free;
    int res = SOInit(&cfg);
    SP_LOG("SOInit returned %i", res);
    assert(res == 0 && "SOInit failed");

    OSCreateThread(&thread, Handle, nullptr, stack + sizeof(stack), sizeof(stack), 31, 0);
    OSResumeThread(&thread);
}

void Restart() {
    {
        ScopeLock<NoInterrupts> lock;
        if (res == 0) {
            res = INT_MIN;
        }
    }
    OSWakeupThread(&queue);
}

} // namespace SP::Net
