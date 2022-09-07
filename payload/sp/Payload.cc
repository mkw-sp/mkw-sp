extern "C" {
#include "Payload.h"
}

#include "sp/Channel.hh"
extern "C" {
#include "sp/Dol.h"
}
extern "C" {
#include "sp/Host.h"
#include "sp/Patcher.h"
}
#include "sp/Rel.hh"
#include "sp/Time.hh"
extern "C" {
#include "sp/keyboard/SIKeyboard.h"
}
#include "sp/net/Net.hh"
extern "C" {
#include "sp/security/Heap.h"
#include "sp/security/Memory.h"
#include "sp/security/Stack.h"
#include "sp/storage/LogFile.h"
}
#include "sp/storage/DecompLoader.hh"
#include "sp/storage/Storage.hh"
extern "C" {
#include "sp/storage/Usb.h"
}

#include <common/Console.hh>
#include <common/VI.hh>
#include <game/host_system/SystemManager.hh>
extern "C" {
#include <libhydrogen/hydrogen.h>
#include <revolution.h>
}

#include <cstring>

extern void (*payload_ctors_start)(void);
extern void (*payload_ctors_end)(void);

namespace SP::Payload {

static void ReturnToLoader() {
    System::SystemManager::ResetDolphinSpeedLimit();
    if (!memcmp(reinterpret_cast<void *>(0x80001804), "STUBHAXX", 8)) {
        if (*reinterpret_cast<u32 *>(0x80001800)) {
            System::SystemManager::LaunchTitle(UINT64_C(0x000100014f484243) /* OHBC */);
            System::SystemManager::LaunchTitle(UINT64_C(0x000100014c554c5a) /* LULZ */);
        } else {
            OSShutdownSystem();
        }
    }
    OSDisableScheduler();
    __OSShutdownDevices(6);
    OSEnableScheduler();
    __OSReturnToMenuForError();
}

static void Init() {
    VI::Init();

    Console::Init();
    Console::Print("MKW-SP v");
    Console::Print(versionInfo.name);
    Console::Print("\n");

    Console::Print("Applying security patches...");
#ifndef GDB_COMPATIBLE
    OSDisableCodeExecOnMEM1Hi16MB();
    OSEnableCodeExecOnPayload();
#endif

#ifdef GDB_COMPATIBLE
    OSSetMEM1ArenaLo((void*)0x809C4FA0);
#else
    OSAllocFromMEM1ArenaLo(Rel_getSize(), 0x20);
#endif

    OSAllocFromMEM1ArenaLo(Payload_getSize(), 0x20);

    Heap_RandomizeMEM1Heaps();
    Heap_RandomizeMEM2Heaps();

    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_1, Dol_getInitSectionStart(), Dol_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_2, Dol_getSdata2SectionStart(), Dol_getSbss2SectionEnd(), OS_PROTECT_PERMISSION_READ);
    Console::Print(" done.\n");

    Console::Print("Initializing RTC...");
    Time::Init();
    Console::Print(" done.\n");

    Console::Print("Initializing host...");
    Host_Init();
    Console::Print(" done.\n");

    Console::Print("Initializing USB...");
    bool usbWasInit = Usb_init();
    assert(usbWasInit);
    Console::Print(" done.\n");

    Console::Print("Initializing network...");
    Net::Init();
    Console::Print(" done.\n");

    Console::Print("Initializing RNG...");
    hydro_init();
    Console::Print(" done.\n");

    Console::Print("Initializing storage...");
    if (!Storage::Init()) {
        Console::Print(" failed!\n");
        Console::Print("Please make sure that an SD or USB device is inserted.\n");
        Console::Print("Trying again in a loop...");
        u32 i;
        for (i = 0; i < 60 && !Storage::Init(); i++) {
            OSSleepMilliseconds(500);
        }
        if (i == 60) {
            Console::Print(" failed!\n");
            Console::Print("Returning to loader...");
            ReturnToLoader();
        }
    }
    Console::Print(" done.\n");

    Console::Print("Initializing log file...");
    LogFile_Init();
    Console::Print(" done.\n");

    // Example output:
    //     --------------------------------
    //     MKW-SP v0.1.4 (Release) REV 15610c0
    //     Region: PAL, System: Dolphin 5.0-15993
    //     Built Mar  2 2022 at 23:22:40, GCC 10.2.0
    //     --------------------------------
    Host_PrintMkwSpInfo(OSReport);

    Console::Print("Initializing concurrent decompressor...");
    Storage::DecompLoader::Init();
    Console::Print(" done.\n");

    Console::Print("Initializing SI keyboard...");
    SIKeyboard_InitSimple();
    Console::Print(" done.\n");

    Console::Print("Initializing channel installer...");
    Channel::Init();
    Console::Print(" done.\n");

    Console::Print("Loading StaticR.rel...");
    bool relWasLoaded = Rel::Load();
    assert(relWasLoaded);
    Console::Print(" done.\n");

    VIInit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    // We don't clear the arena in OSInit because the payload is already copied at that point, and
    // the XFB would turn green, but some code expects it to be zeroed.
    auto *mem1Lo = reinterpret_cast<u8 *>(OSGetMEM1ArenaLo());
    auto *mem1Hi = reinterpret_cast<u8 *>(OSGetMEM1ArenaHi());
    memset(OSGetMEM1ArenaLo(), 0, mem1Hi - mem1Lo);

    System::SystemManager::ResetDolphinSpeedLimit();
    System::RichPresenceManager::Init();
}

static void Run() {
    for (void (**ctor)(void) = &payload_ctors_start; ctor < &payload_ctors_end; ctor++) {
        (*ctor)();
    }
    Stack_InitCanary();
    Stack_RelocateMainThreadStackToMEM1End();
#ifndef GDB_COMPATIBLE
    Stack_DoLinkRegisterPatches(reinterpret_cast<u32 *>(Dol_getTextSectionStart()),
            reinterpret_cast<u32 *>(Dol_getTextSectionEnd()));
#endif
    Patcher_patch(PATCHER_BINARY_DOL);
}

} // namespace SP::Payload

extern "C" void Payload_Init() {
    SP::Payload::Init();
}

extern "C" __attribute__((no_stack_protector, section("first"))) void Payload_Run() {
    SP::Payload::Run();
}
