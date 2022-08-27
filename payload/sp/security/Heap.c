#include "Heap.h"

#include <revolution.h>
#include <vendor/libhydrogen/hydrogen.h>

#define RANDOMIZATION_SPACE_BYTES (1024 * 16)

void Heap_RandomizeMEM1Heaps(void) {
    const u32 random = hydro_random_uniform(RANDOMIZATION_SPACE_BYTES);
    OSAllocFromMEM1ArenaLo(random, 32);
    OSAllocFromMEM1ArenaHi(RANDOMIZATION_SPACE_BYTES - random, 32);
}

void Heap_RandomizeMEM2Heaps(void) {
    const u32 random = hydro_random_uniform(RANDOMIZATION_SPACE_BYTES);
    OSAllocFromMEM2ArenaLo(random, 32);
    OSAllocFromMEM2ArenaHi(RANDOMIZATION_SPACE_BYTES - random, 32);
}
