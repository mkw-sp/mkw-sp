#include "Host.h"

#include <Common.h>
#include <revolution.h>
#include <revolution/ios.h>
#include <sp/IOSDolphin.h>
#include <sp/Version.h>
#include <stdio.h>
#include <string.h>

static bool sHostIsInit = false;
static HostPlatform sPlatform = HOST_UNKNOWN;
static char sDolphinTag[64] = "Not dolphin";

#ifndef PLATFORM_EMULATOR
static void DetectPlatform(void) {
    {
        IOSDolphin iosDolphin = IOSDolphin_Open();
        if (iosDolphin >= 0) {
            sPlatform = HOST_DOLPHIN;
            IOSDolphin_VersionQuery q = IOSDolphin_GetVersion(iosDolphin);
            if (!q.hasValue) {
                OSReport("[HOSTPLATFORM] Failed GetVersion query\n");
                sPlatform = HOST_DOLPHIN_UNKNOWN;
            } else {
                memcpy(sDolphinTag, q.version,
                        MIN(sizeof(sDolphinTag), sizeof(q.version)));
                sDolphinTag[sizeof(sDolphinTag) - 1] = '\0';
            }
            IOSDolphin_Close(iosDolphin);
            return;
        }

        s32 iosSys = IOS_Open("/sys", 1);
        if (iosSys == -106) {
            // Old dolphin version
            sPlatform = HOST_DOLPHIN_UNKNOWN;
            return;
        }
        if (iosSys >= 0) {
            IOS_Close(iosSys);
        }
    }

    {
        const u32 acrReg = *(volatile u32 *)0xCD8005A0;

        switch (acrReg >> 16) {
        case 0xCAFE:
            sPlatform = HOST_CAFE;
            return;
        case 0:
            sPlatform = HOST_REVOLUTION;
            return;
        }
    }

    sPlatform = HOST_UNKNOWN;
}
#endif

void Host_Init(void) {
    if (sHostIsInit)
        return;
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
    (*func)("MKW-SP v" BUILD_TYPE_STR "\n");

    char system[128];
    memset(system, 0, sizeof(system));

    switch (Host_GetPlatform()) {
    case HOST_REVOLUTION:
        snprintf(system, sizeof(system), "Wii");
        break;
    case HOST_CAFE:
        snprintf(system, sizeof(system), "WiiU");
        break;
    case HOST_DOLPHIN_UNKNOWN:
        snprintf(system, sizeof(system), "Dolphin (Unsupported)");
        break;
    case HOST_DOLPHIN:
        snprintf(system, sizeof(system), "Dolphin %s", Host_GetDolphinTag());
        break;
    case HOST_WINDOWS:
        snprintf(system, sizeof(system), "Windows");
        break;
    case HOST_APPLE:
        snprintf(system, sizeof(system), "Apple");
        break;
    case HOST_LINUX:
        snprintf(system, sizeof(system), "Linux");
        break;
    case HOST_UNKNOWN:
        snprintf(system, sizeof(system), "Unknown host");
        break;
    default:
        snprintf(system, sizeof(system), "Invalid host");
        break;
    }

    if (Host_IsGeckoEnabled()) {
        const int len = strnlen(system, sizeof(system));
        snprintf(system + len, sizeof(system) - len, "*");
    }

    (*func)("Region: %s, System: %s\n", region, system);
    (*func)("Built " __DATE__ " at " __TIME__ ", " CC_STR "\n");
    (*func)("--------------------------------\n");
}
