#include "Host.h"

#include <Common.h>
#include <revolution.h>
#include <revolution/ios.h>
#include <sp/IOSDolphin.h>
#include <sp/Version.h>
#include <string.h>

static bool sHostIsInit = false;
static HostPlatform sPlatform = HOST_UNKNOWN;
static char sDolphinTag[64] = "Not dolphin";

void Host_Init(void) {
    if (sHostIsInit)
        return;

    sHostIsInit = true;

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

    const u32 acrReg = *(volatile u32 *)0xCD8005A0;

    switch (acrReg >> 16) {
    case 0xCAFE:
        sPlatform = HOST_CAFE;
        return;
    case 0:
        sPlatform = HOST_REVOLUTION;
        return;
    }

    sPlatform = HOST_UNKNOWN;
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
    return *(volatile u8 *)0x800018A8 == 0x94;
}

const char *Host_GetRegionString(void) {
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
}

void Host_PrintMkwSpInfo(PrintfFunction *func) {
    Host_Init();

    const char *region = Host_GetRegionString();

    (*func)("--------------------------------\n");
    (*func)("MKW-SP v" BUILD_TYPE_STR "\nRegion: %s, System: ", region);

    switch (Host_GetPlatform()) {
    case HOST_REVOLUTION:
        (*func)("Wii");
        break;
    case HOST_CAFE:
        (*func)("WiiU");
        break;
    case HOST_DOLPHIN_UNKNOWN:
        (*func)("Dolphin (Unsupported)");
        break;
    case HOST_DOLPHIN:
        (*func)("Dolphin %s", Host_GetDolphinTag());
        break;
    case HOST_UNKNOWN:
        (*func)("Unknown host");
        break;
    default:
        (*func)("Invalid host");
        break;
    }

    if (Host_IsGeckoEnabled()) {
        (*func)("*");
    }

    (*func)("\n");

    (*func)("Built " __DATE__ " at " __TIME__ ", " CC_STR "\n");

    if (_HAS_GIT_CHANGES) {
        (*func)("Changed files: " _GIT_CHANGED_FILES "\n");
    }

    (*func)("--------------------------------\n");
}
