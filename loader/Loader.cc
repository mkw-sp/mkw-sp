#include "Apploader.hh"

#include <common/Clock.hh>
#include <common/Console.hh>
#include <common/DCache.hh>
#include <common/ICache.hh>
#include <common/VI.hh>

#include <cstring>

namespace Loader {

typedef void (*PayloadEntryFunc)(void);

extern "C" const u8 payloadP[];
extern "C" const u32 payloadPSize;
extern "C" const u8 payloadE[];
extern "C" const u32 payloadESize;
extern "C" const u8 payloadJ[];
extern "C" const u32 payloadJSize;
extern "C" const u8 payloadK[];
extern "C" const u32 payloadKSize;

void Run() {
    VI::Init();

    Console::Init();
    Console::Print("MKW-SP v");
    Console::Print(versionInfo.name);
    Console::Print("\n");

    std::optional<Apploader::GameEntryFunc> gameEntry;
    {
        IOS::DI di;

        while (!(gameEntry = Apploader::LoadAndRun(di))) {
            if (!di.isInserted()) {
                Console::Print("Please insert a Mario Kart Wii disc.\n");

                while (!di.isInserted()) {
                    Clock::WaitMilliseconds(100);
                }
            }

            Clock::WaitMilliseconds(100);

            Console::Print("Resetting disc interface...");
            di.reset();
            Console::Print(" done.\n");
        }
    }

    void *payloadDst;
    const void *payloadSrc;
    u32 payloadSize;
    switch (REGION) {
    case REGION_P:
#ifdef GDB_COMPATIBLE
        payloadDst = reinterpret_cast<void *>(0x809C4FA0);
#else
        payloadDst = reinterpret_cast<void *>(0x8076db60);
#endif
        payloadSrc = &payloadP;
        payloadSize = payloadPSize;
        break;
    case REGION_E:
        payloadDst = reinterpret_cast<void *>(0x80769400);
        payloadSrc = &payloadE;
        payloadSize = payloadESize;
        break;
    case REGION_J:
        payloadDst = reinterpret_cast<void *>(0x8076cca0);
        payloadSrc = &payloadJ;
        payloadSize = payloadJSize;
        break;
    case REGION_K:
        payloadDst = reinterpret_cast<void *>(0x8075bfe0);
        payloadSrc = &payloadK;
        payloadSize = payloadKSize;
        break;
    default:
        Console::Print("Region detection failed!");
        return;
    }

    Console::Print("Copying payload...");
    memcpy(payloadDst, payloadSrc, payloadSize);
    DCache::Flush(payloadDst, payloadSize);
    ICache::Invalidate(payloadDst, payloadSize);
    Console::Print(" done.\n");

    Console::Print("Applying patches...");
    PayloadEntryFunc payloadEntry = reinterpret_cast<PayloadEntryFunc>(payloadDst);
    payloadEntry();
    Console::Print(" done.\n");

    (*gameEntry)();
}

} // namespace Loader

extern "C" void Loader_Run() {
    Loader::Run();
}
