#include "Net.hh"

#include "sp/ScopeLock.hh"

#include <egg/core/eggExpHeap.hh>

#include <climits>

namespace SP::Net {

static Mutex mutex;
static OSThreadQueue queue;
static std::array<u8, 0x1000 /* 4 KiB*/> stack;
static OSThread thread;
static int res = INT_MIN;
static EGG::ExpHeap *heap;
static constexpr u32 heapSize = 0x10000;

void *Alloc(s32 size) {
    return heap->alloc(size, 32);
}

void Free(void *block, s32 /* size */) {
    heap->free(block);
}

static void *Alloc(u32 /* id */, s32 size) {
    return Alloc(size);
}

static void Free(u32 /* id */, void *block, s32 size) {
    Free(block, size);
}

static void *Handle(void * /* arg */) {
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
    heap = EGG::ExpHeap::Create(OSAllocFromMEM2ArenaLo(heapSize, 32), heapSize, 1);

    SOLibraryConfig libraryConfig{Alloc, Free};
    assert(SOInit(&libraryConfig) == SO_OK);

    OSCreateThread(&thread, Handle, nullptr, stack.data() + stack.size(), stack.size(), 31, 0);
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
