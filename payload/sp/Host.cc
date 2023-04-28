extern "C" {
#include "Host.h"

#include <Common.h>

#include <revolution/ios.h>
#include <revolution/nwc24/NWC24Utils.h>
#include <revolution/os.h>
#include <sp/Version.h>

#include <stdio.h>
#include <string.h>
}

#include <sp/IOSDolphin.hh>

static bool sHostIsInit = false;
static HostPlatform sPlatform = HOST_UNKNOWN;
static char sDolphinTag[HOST_PLATFORM_BUFFER_SIZE] = "Not dolphin";
#undef PLATFORM_EMULATOR
#ifndef PLATFORM_EMULATOR
const RawCPU_ID sCpuId_PrehistoricDolphin = {0, 0, 0, 0};
const RawCPU_ID sCpuId_Dolphin = {0xd96e200, 0x1840c00d, 0x82bb08e8, 0};

RawCPU_ID Host_GetCPUID(void) {
    u32 _39c = 0, _39d = 0, _39e = 0, _39f = 0;
    __asm__ volatile("mfspr %0, 0x39c" : "=r"(_39c));
    __asm__ volatile("mfspr %0, 0x39d" : "=r"(_39d));
    __asm__ volatile("mfspr %0, 0x39e" : "=r"(_39e));
    __asm__ volatile("mfspr %0, 0x39f" : "=r"(_39f));
    return (RawCPU_ID){
            ._924 = _39c,
            .ECID_H = _39d,
            .ECID_M = _39e,
            .ECID_L = _39f,
    };
}

void Host_SetCPUID(RawCPU_ID id) {
    u32 _39c = id._924, _39d = id.ECID_H, _39e = id.ECID_M, _39f = id.ECID_L;
    __asm__ volatile("mtspr 0x39c, %0" : "=r"(_39c));
    __asm__ volatile("mtspr 0x39d, %0" : "=r"(_39d));
    __asm__ volatile("mtspr 0x39e, %0" : "=r"(_39e));
    __asm__ volatile("mtspr 0x39f, %0" : "=r"(_39f));
}

static void DetectPlatform(void) {
    {
        if (SP::IOSDolphin::Open()) {
            sPlatform = HOST_DOLPHIN;
            auto q = SP::IOSDolphin::GetVersion();
            if (!q.has_value()) {
                OSReport("[HOSTPLATFORM] Failed GetVersion query\n");
                sPlatform = HOST_DOLPHIN_UNKNOWN;
            } else {
                const char dolphinTagFormatString[] = "Dolphin %s";
                snprintf(sDolphinTag, sizeof(sDolphinTag), dolphinTagFormatString, q->data());
            }
            return;
        }

        RawCPU_ID cpuid = Host_GetCPUID();

        if (!memcmp(&cpuid, &sCpuId_Dolphin, sizeof(cpuid))) {
            sPlatform = HOST_DOLPHIN_UNKNOWN;
            return;
        }
        if (!memcmp(&cpuid, &sCpuId_PrehistoricDolphin, sizeof(cpuid))) {
            sPlatform = HOST_DOLPHIN_PREHISTORIC;
            return;
        }
    }

    {
        s32 macaddr = IOS_Open("/title/00000001/00000002/data/macaddr.bin", IPC_OPEN_READ);
        if (macaddr >= 0) {
            IOS_Close(macaddr);
            sPlatform = HOST_WII_MINI;
            return;
        }
    }

    {
        if (OSIsTitleInstalled(UINT64_C(0x0000000100000200))) {
            sPlatform = HOST_CAFE;
        } else {
            sPlatform = HOST_REVOLUTION;
        }
    }
}
#endif

static const char *GetWiiTag(void) {
    if ((OSGetConsoleType() & OS_CONSOLE_MASK) == OS_CONSOLE_MASK_RETAIL) {
        return "Wii (Retail)";
    } else {
        return "Wii (Development)";
    }
}

void Host_Init(void) {
    if (sHostIsInit) {
        return;
    }
    sHostIsInit = true;

#if defined(_WIN32)
    sPlatform = HOST_WINDOWS;
#elif defined(__APPLE__)
    sPlatform = HOST_APPLE;
#elif defined(__linux__)
    sPlatform = HOST_LINUX;
#else
    DetectPlatform();
#endif
}

const char *Host_GetDolphinTag(void) {
    Host_Init();
    return sDolphinTag;
}

HostPlatform Host_GetPlatform(void) {
    Host_Init();
    return sPlatform;
}

const char *Host_GetPlatformString(void) {
    switch (Host_GetPlatform()) {
    case HOST_REVOLUTION:
        return GetWiiTag();
    case HOST_WII_MINI:
        return "Wii Mini";
    case HOST_CAFE:
        return "Wii U";
    case HOST_DOLPHIN_PREHISTORIC:
        return "Ancient Dolphin (Unsupported)";
    case HOST_DOLPHIN_UNKNOWN:
        return "Dolphin (Unsupported)";
    case HOST_DOLPHIN:
        return Host_GetDolphinTag();
    case HOST_WINDOWS:
        return "Windows";
    case HOST_APPLE:
        return "Apple";
    case HOST_LINUX:
        return "Linux";
    case HOST_UNKNOWN:
        return "Unknown host";
    default:
        return "Invalid host";
    }
}

bool Host_IsGeckoEnabled(void) {
#ifdef PLATFORM_EMULATOR
    return false;
#else
    return *(volatile u8 *)0x800018A8 == 0x94;
#endif
}

const char *Host_GetRegionString(void) {
#ifdef PLATFORM_EMULATOR
    return "PAL";
#else
    switch (REGION) {
    case REGION_P:
        return "PAL";
    case REGION_E:
        return "NTSC-U";
    case REGION_J:
        return "NTSC-J";
    case REGION_K:
        return "NTSC-K";
    default:
        return "?";
    }
#endif
}

void Host_PrintMkwSpInfo(PrintfFunction *func) {
    Host_Init();

    const char *region = Host_GetRegionString();

    (*func)("--------------------------------\n");
    (*func)("MKW-SP v%s\n", versionInfo.name);

    char system[HOST_PLATFORM_BUFFER_SIZE];
    NWC24iStrLCpy(system, Host_GetPlatformString(), sizeof(system));

    if (Host_IsGeckoEnabled()) {
        const s32 len = strnlen(system, sizeof(system));
        snprintf(system + len, sizeof(system) - len, "*");
    }

    (*func)("Region: %s, System: %s\n", region, system);
    (*func)("Built " __DATE__ " at " __TIME__ ", " CC_STR "\n");
    (*func)("--------------------------------\n");
}
