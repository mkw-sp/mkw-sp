#include "Apploader.hh"

#include <common/Clock.hh>
#include <common/Console.hh>
#include <common/DCache.hh>
#include <common/ES.hh>
#include <common/FS.hh>
#include <common/ICache.hh>
#include <common/Paths.hh>
#include <common/VI.hh>

#include <cstring>
#include <iterator>

extern "C" void (*ctors_start)(void);
extern "C" void (*ctors_end)(void);

extern "C" const u8 payloadP[];
extern "C" const u32 payloadPSize;
extern "C" const u8 payloadE[];
extern "C" const u32 payloadESize;
extern "C" const u8 payloadJ[];
extern "C" const u32 payloadJSize;
extern "C" const u8 payloadK[];
extern "C" const u32 payloadKSize;

namespace Loader {

typedef void (*PayloadEntryFunc)(void);

#define TMP_CONTENTS_PATH "/tmp/contents.arc"

static void *contents = reinterpret_cast<void *>(0x80100000);

static bool ReloadIOS(u64 titleID) {
    IOS::ES es;
    if (!es.ok()) {
        return false;
    }

    auto count = es.getTicketViewCount(titleID);
    if (!count) {
        return false;
    }

    alignas(0x20) IOS::ES::TicketView views[8];
    if (!es.getTicketViews(titleID, *count, views)) {
        return false;
    }

    *(volatile u32 *)0xc0003140 = 0;

    if (!es.launchTitle(titleID, &views[0])) {
        return false;
    }

    while (*(volatile u32 *)0xc0003140 == 0) {
        Clock::WaitMilliseconds(1);
    }

    IOS::Init();

    return true;
}

static bool CopyNANDLoader(IOS::FS &fs) {
    alignas(0x20) const char *pathP2 = "/title/00010008/48414c50/content/0000000b.app";
    alignas(0x20) const char *pathE2 = "/title/00010008/48414c45/content/0000000b.app";
    alignas(0x20) const char *pathJ2 = "/title/00010008/48414c4a/content/0000000b.app";
    alignas(0x20) const char *pathK2 = "/title/00010008/48414c4b/content/00000006.app";
    alignas(0x20) const char *pathP1 = "/title/00010008/48414c50/content/00000006.app";
    alignas(0x20) const char *pathE1 = "/title/00010008/48414c45/content/00000006.app";
    alignas(0x20) const char *pathJ1 = "/title/00010008/48414c4a/content/00000006.app";
    const char *const paths[] = {pathP2, pathE2, pathJ2, pathK2, pathP1, pathE1, pathJ1};
    for (u32 i = 0; i < std::size(paths); i++) {
        if (fs.copyFile(paths[i], ALIGNED_STRING(TMP_NAND_LOADER_PATH))) {
            return true;
        }
    }
    fs.erase(ALIGNED_STRING(TMP_NAND_LOADER_PATH));
    return false;
}

std::optional<Apploader::GameEntryFunc> Run() {
    VI::Init();

    Console::Init();
    Console::Print("MKW-SP v");
    Console::Print(versionInfo.name);
    Console::Print("\n");
    Console::Print("\n");

    std::optional<u32> contentsSize{};
    if (versionInfo.type != BUILD_TYPE_RELEASE) {
        Console::Print("Saving contents.arc...");
        IOS::FS fs;
        if (!fs.ok()) {
            Console::Print(" failed!\n");
            return {};
        }
        contentsSize = fs.readFile(ALIGNED_STRING(TMP_CONTENTS_PATH), contents, 0x900000);
        if (!contentsSize) {
            Console::Print(" failed!\n");
            return {};
        }
        Console::Print(" done.\n");
    }

    Console::Print("Reloading IOS...");
    if (!ReloadIOS(UINT64_C(0x000000010000003a))) {
        Console::Print(" failed!\n");
        return {};
    }
    Console::Print(" done.\n");

    if (versionInfo.type != BUILD_TYPE_RELEASE) {
        Console::Print("Restoring contents.arc...");
        IOS::FS fs;
        if (!fs.ok()) {
            Console::Print(" failed!\n");
            return {};
        }
        if (!fs.writeFile(ALIGNED_STRING(TMP_CONTENTS_PATH), contents, *contentsSize)) {
            Console::Print(" failed!\n");
            return {};
        }
        Console::Print(" done.\n");
    }

    Console::Print("Escalating privileges...");
    if (!IOS::EscalatePrivileges()) {
        Console::Print(" failed!\n");
        return {};
    }
    Console::Print(" done.\n");

    Console::Print("Initializing FS...");
    IOS::FS fs;
    if (!fs.ok()) {
        Console::Print(" failed!\n");
        return {};
    }
    Console::Print(" done.\n");

    if (versionInfo.type == BUILD_TYPE_RELEASE) {
        Console::Print("Copying NAND loader...");
        if (CopyNANDLoader(fs)) {
            Console::Print(" done.\n");
        } else {
            Console::Print(" failed!\n");
        }
    }

    Console::Print("Importing new common key...");
    IOS::ImportNewCommonKey();
    Console::Print(" done.\n");

    Console::Print("Deescalating privileges...");
    IOS::DeescalatePrivileges();
    Console::Print(" done.\n");

    std::optional<Apploader::GameEntryFunc> gameEntry;
    {
        IOS::DI di;

        while (!(gameEntry = Apploader::LoadAndRun(di))) {
            if (!di.isInserted()) {
                if (IOS::IsDolphin()) {
                    Console::Print(
                            "\nInsert the Mario Kart Wii disc\nby right-clicking the game in "
                            "the game list \nand select \"Change Disc\".\n\n"
                            "To avoid this in the future, select \n\"Set as Default ISO\" as well.\n");
                } else {
                    Console::Print("Please insert a Mario Kart Wii disc.\n");
                }

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
        payloadDst = reinterpret_cast<void *>(0x8076F000);
#endif
        payloadSrc = &payloadP;
        payloadSize = payloadPSize;
        break;
    case REGION_E:
        payloadDst = reinterpret_cast<void *>(0x8076A000);
        payloadSrc = &payloadE;
        payloadSize = payloadESize;
        break;
    case REGION_J:
        payloadDst = reinterpret_cast<void *>(0x8076E000);
        payloadSrc = &payloadJ;
        payloadSize = payloadJSize;
        break;
    case REGION_K:
        payloadDst = reinterpret_cast<void *>(0x8075D000);
        payloadSrc = &payloadK;
        payloadSize = payloadKSize;
        break;
    default:
        Console::Print("Region detection failed!");
        return {};
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

    return gameEntry;
}

} // namespace Loader

extern "C" void Loader_Run() {
    for (void (**ctor)(void) = &ctors_start; ctor < &ctors_end; ctor++) {
        (*ctor)();
    }

    auto gameEntry = Loader::Run();
    if (gameEntry) {
        (*gameEntry)();
    }
}
