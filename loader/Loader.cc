#include "Apploader.hh"

#include "loader/Dol.hh"

#include <common/Clock.hh>
#include <common/Console.hh>
#include <common/DCache.hh>
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

    u16 iosNumber = IOS::GetNumber();
    if (iosNumber != 58 && iosNumber != 59) {
        Console::Print("In order for MKW-SP to work properly, IOS58 or IOS59 must be used.\n");
        Console::Print("Before launching MKW-SP, ensure that IOS58 or IOS59 is loaded!");
        return {};
    }

    Console::Print("Escalating privileges...");
    if (!IOS::EscalatePrivileges(true)) {
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
                            "To avoid this in the future, select \n\"Set as Default ISO\" as "
                            "well.\n");
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

    std::optional<bool> isDolClean = Dol::IsClean();
    if (!isDolClean.has_value()) {
        Console::Print("Unsupported game region detected!");
        return {};
    }
    if (!*isDolClean) {
        Console::Print(
                "Please ensure that the file 'main.dol' is not modified\n"
                "in any capacity!");
        return {};
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
