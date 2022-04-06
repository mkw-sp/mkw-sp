#include "Apploader.h"
#include "Cache.h"
#include "Delay.h"
#include "Di.h"
#include "Stack.h"
#include "Vi.h"

#include <string.h>

typedef void (*PayloadEntryFunc)(void);

extern const void payloadP;
extern const u32 payloadPSize;
extern const void payloadE;
extern const u32 payloadESize;
extern const void payloadJ;
extern const u32 payloadJSize;
extern const void payloadK;
extern const u32 payloadKSize;

void Loader_run(void) {
    Vi_init();

    while (!Di_init()) {
        mdelay(100);
    }

    GameEntryFunc gameEntry;
    while (!Apploader_loadAndRun(&gameEntry)) {
        mdelay(100);

        if (Di_isInserted()) {
            Di_reset();
        }
    }

    Di_deinit();

    Stack_RandomizeMainThreadStackPointer();

    void *payloadDst;
    const void *payloadSrc;
    u32 payloadSize;
    switch (REGION) {
    case REGION_P:
#ifdef GDB_COMPATIBLE
        payloadDst = (void *)0x809C4FA0;
#else
        payloadDst = (void *)0x8076db60;
#endif
        payloadSrc = &payloadP;
        payloadSize = payloadPSize;
        break;
    case REGION_E:
        payloadDst = (void *)0x80769400;
        payloadSrc = &payloadE;
        payloadSize = payloadESize;
        break;
    case REGION_J:
        payloadDst = (void *)0x8076cca0;
        payloadSrc = &payloadJ;
        payloadSize = payloadJSize;
        break;
    case REGION_K:
        payloadDst = (void *)0x8075bfe0;
        payloadSrc = &payloadK;
        payloadSize = payloadKSize;
        break;
    default:
        // TODO tell the user about it
        while (true)
            ;
    }

    memcpy(payloadDst, payloadSrc, payloadSize);
    DCFlushRange(payloadDst, payloadSize);
    ICInvalidateRange(payloadDst, payloadSize);

    PayloadEntryFunc payloadEntry = payloadDst;
    payloadEntry();

    gameEntry();
}
