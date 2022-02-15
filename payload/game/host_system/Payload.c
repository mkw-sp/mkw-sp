#include "Patcher.h"

#include <revolution.h>

#include <stdint.h>

u32 *hits;
static OSAlarm alarm;

static void handler(OSAlarm *UNUSED(alarm), OSContext *context) {
    u32 index = (context->srr0 - 0x80000000) / 4;
    if (hits[index] < UINT32_MAX) {
        hits[index]++;
    }
}

__attribute__((section("first"))) void start(void) {
    // TODO add OSAllocFromMEM2ArenaHi
    void *hi = OSGetMEM2ArenaHi();
    hi -= 0x800000;
    OSSetMEM2ArenaHi(hi);
    hits = hi;
    OSReport("[PERF] Hit buffer initialized at %p.\n", hits);
    OSSetPeriodicAlarm(&alarm, 0, OSMicrosecondsToTicks(100), handler);

    Patcher_patch(PATCHER_BINARY_DOL);
}
