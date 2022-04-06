#include "Apploader.h"
#include "Cache.h"
#include "Console.h"
#include "Delay.h"
#include "Di.h"
#include "Ios.h"
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
    // On console, bad stuff seems to happen when writing to the XFB, presumably when some cache
    // lines are written back to main memory. Prevent that by completely emptying the dcache.
    DCInvalidateRange((void *)0x80000000, 0x1800000);

    Vi_init();

    Console_init();
    Console_printString("MKW-SP Loader\n");

    Ios_init();

    Console_printString("Initializing disc interface...");
    while (!Di_init()) {
        mdelay(100);
    }
    Console_printString(" done.\n");

    GameEntryFunc gameEntry;
    while (!Apploader_loadAndRun(&gameEntry)) {
        if (!Di_isInserted()) {
            Console_printString("Please insert a Mario Kart Wii disc.\n");

            while (!Di_isInserted()) {
                mdelay(100);
            }
        }

        mdelay(100);

        if (Di_isInserted()) {
            Console_printString("Resetting disc interface...");
            Di_reset();
            Console_printString(" done.\n");
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
        Console_printString("Region detection failed!");
        return;
    }

    Console_printString("Copying payload...");
    memcpy(payloadDst, payloadSrc, payloadSize);
    DCFlushRange(payloadDst, payloadSize);
    ICInvalidateRange(payloadDst, payloadSize);
    Console_printString(" done.\n");
 
    Console_printString("Applying patches...");
    PayloadEntryFunc payloadEntry = payloadDst;
    payloadEntry();
    Console_printString(" done.\n");

    Vi_deinit();

    gameEntry();
}
