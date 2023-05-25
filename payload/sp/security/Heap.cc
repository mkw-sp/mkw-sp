#include "Heap.hh"

extern "C" {
#include <revolution.h>
#include <vendor/libhydrogen/hydrogen.h>
}

namespace SP::Heap {

#define RANDOMIZATION_SPACE_BYTES (1024 * 16)

void RandomizeMEM1Heaps() {
    const u32 random = hydro_random_uniform(RANDOMIZATION_SPACE_BYTES);

    OSAllocFromMEM1ArenaLo(random, 32);
    OSAllocFromMEM1ArenaHi(RANDOMIZATION_SPACE_BYTES - random, 32);
}

void RandomizeMEM2Heaps() {
    const u32 random = hydro_random_uniform(RANDOMIZATION_SPACE_BYTES);

    OSAllocFromMEM2ArenaLo(random, 32);
    OSAllocFromMEM2ArenaHi(RANDOMIZATION_SPACE_BYTES - random, 32);
}

} // namespace SP::Heap
