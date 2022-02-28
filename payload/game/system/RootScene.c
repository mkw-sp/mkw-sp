#include "RootScene.h"
#include <revolution.h>
#include <sp/Slab.h>
#include <string.h>

#ifdef LOADING_TIME_DEBUG
#include <sp/FlameGraph.h>
#define LOADING_SECTION(s) SIMPLE_PERF_NAMED(s)
#else
#define LOADING_SECTION(s)
#endif

static bool sNetIsInit = false;
EGG_Heap *spSocketHeap = NULL;
static OSMutex sSocketMutex;

enum {
    NET_HEAP_SIZE = sizeof(NetSlabs) + 5000,
};

static void *so_alloc(u32 id, s32 size) {
    assert(spSocketHeap);

    OSLockMutex(&sSocketMutex);

    // Prefer the slab allocator
    {
        void *slab_res = TryAllocFromSlabs(size);
        if (slab_res != NULL) {
            OSUnlockMutex(&sSocketMutex);
            return slab_res;
        }
    }

    // Fall back to ExpHeap

    SP_LOG("[SO] Alloc ID(%i) %i bytes\n", id, size);
    void *res = EGG_Heap_alloc(size, 32, spSocketHeap);
    SP_LOG("[SO] Alloc @ %p\n", res);

    OSUnlockMutex(&sSocketMutex);
    return res;
}
static void so_free(u32 UNUSED(id), void *ptr, s32 size) {
    assert(spSocketHeap);

    OSLockMutex(&sSocketMutex);

    // TODO: In theory, we do not need to lock a mutex to free from the slab allocator (?)
    if (TryFreeFromSlabs(ptr, size)) {
        OSUnlockMutex(&sSocketMutex);
        return;
    }

    OSReport("[SO] Free %i bytes\n", size);
    EGG_Heap_free(ptr, spSocketHeap);
    OSUnlockMutex(&sSocketMutex);
}

static void RootScene_initNet(RootScene *scn) {
    if (sNetIsInit)
        return;

    LOADING_SECTION("Initializing network");

    OSInitMutex(&sSocketMutex);

    {
        LOADING_SECTION("Creating linked-list heap");

        spSocketHeap = EGG_ExpHeap_create(
                NET_HEAP_SIZE, scn->heapCollection.heaps[1], /* attr= */ 0);
        assert(spSocketHeap && "Failed to allocate socket heap");
    }

    {
        LOADING_SECTION("-> Creating slab allocator");

        sSlabs = EGG_Heap_alloc(sizeof(NetSlabs), 4, spSocketHeap);
        assert(sSlabs && "Failed to create slab allocator");
        memset(sSlabs, 0, sizeof(*sSlabs));
    }

    {
        LOADING_SECTION("Initializing Socket library");

        SOLibraryConfig cfg;
        cfg.alloc = so_alloc;
        cfg.free = so_free;
        int res = SOInit(&cfg);
        SP_LOG("SOInit returned %i", res);
        assert(res == 0 && "SOInit failed");
    }

    sNetIsInit = true;
}

// Memory layout isn't randomized here
void RootScene_allocateEx(RootScene *scn) {
    RootScene_initNet(scn);
}

PATCH_B(RootScene_allocate + 0xb64, RootScene_allocateEx);
