#include "Heap.h"

#include <revolution.h>
#include <vendor/libhydrogen/hydrogen.h>

#define RANDOMIZATION_SPACE_BYTES (1024 * 8)

void Heap_RandomizeMEM1Heaps(void) {
    OSAllocFromMEM1ArenaLo(hydro_random_uniform(RANDOMIZATION_SPACE_BYTES), 32);
    OSAllocFromMEM1ArenaHi(hydro_random_uniform(RANDOMIZATION_SPACE_BYTES), 32);
}

void Heap_RandomizeMEM2Heaps(void) {
    OSAllocFromMEM2ArenaLo(hydro_random_uniform(RANDOMIZATION_SPACE_BYTES), 32);
    OSAllocFromMEM2ArenaHi(hydro_random_uniform(RANDOMIZATION_SPACE_BYTES), 32);
}
