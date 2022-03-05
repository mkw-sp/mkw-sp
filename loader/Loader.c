#include "Apploader.h"
#include "Cache.h"
#include "Delay.h"
#include "Di.h"
#include "Es.h"
#include "Ios.h"
#include "Stack.h"

#include <stdalign.h>
#include <string.h>

extern const void payloadP;
extern const u32 payloadPSize;
extern const void payloadE;
extern const u32 payloadESize;
extern const void payloadJ;
extern const u32 payloadJSize;
extern const void payloadK;
extern const u32 payloadKSize;

// The payload needs this to reserve the memory on the arena
extern u32 payloadSize;

static bool isDolphin(void) {
    // Dolphin 5.0-11186 and later
    alignas(0x20) char dolphinPath[] = "/dev/dolphin";
    s32 dolphinFd = Ios_open(dolphinPath, 0);
    if (dolphinFd >= 0) {
        Ios_close(dolphinFd);
        return true;
    }

    // Older versions
    alignas(0x20) char sysPath[] = "/sys";
    s32 sysFd = Ios_open(sysPath, 1);
    if (sysFd == -106) {
        return true;
    }
    if (sysFd >= 0) {
        Ios_close(sysFd);
    }

    return false;
}

void Loader_run(void) {
    if (isDolphin()) {
        while (!Es_init()) {
            mdelay(100);
        }

        TicketView view = { 0 }; // Dolphin doesn't care
        Es_launchTitle(0x000000010000003b /* IOS59 */, &view);

        Es_deinit();
    }

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

    RandomizeMainThreadStackPointer();

    void *payloadDst;
    const void *payloadSrc;
    u32 *hook; // At the end of OSInit
    u32 *ClearArena;
    switch (REGION) {
    case REGION_P:
        payloadDst = (void *)0x8076db60;
        payloadSrc = &payloadP;
        payloadSize = payloadPSize;
        hook = (u32 *)0x801a00dc;
        ClearArena = (u32 *)0x8019ff34;
        break;
    case REGION_E:
        payloadDst = (void *)0x80769400;
        payloadSrc = &payloadE;
        payloadSize = payloadESize;
        hook = (u32 *)0x801a003c;
        ClearArena = (u32 *)0x8019fe94;
        break;
    case REGION_J:
        payloadDst = (void *)0x8076cca0;
        payloadSrc = &payloadJ;
        payloadSize = payloadJSize;
        hook = (u32 *)0x8019fffc;
        ClearArena = (u32 *)0x8019fe54;
        break;
    case REGION_K:
        payloadDst = (void *)0x8075bfe0;
        payloadSrc = &payloadK;
        payloadSize = payloadKSize;
        hook = (u32 *)0x801a0438;
        ClearArena = (u32 *)0x801a0290;
        break;
    default:
        // TODO tell the user about it
        while (true);
    }

    memcpy(payloadDst, payloadSrc, payloadSize);
    DCFlushRange(payloadDst, payloadSize);
    ICInvalidateRange(payloadDst, payloadSize);

    *hook = 0x12 << 26 | (((u32)payloadDst - (u32)hook) & 0x3fffffc);
    DCFlushRange(hook, sizeof(u32));
    ICInvalidateRange(hook, sizeof(u32));

    *ClearArena = 0x60000000;
    DCFlushRange(ClearArena, sizeof(u32));
    ICInvalidateRange(ClearArena, sizeof(u32));

    gameEntry();
}
