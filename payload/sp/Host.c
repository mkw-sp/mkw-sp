#include "Host.h"

#include <Common.h>
#include <revolution.h>
#include <revolution/ios.h>
#include <sp/IOSDolphin.h>
#include <string.h>

static char sDolphinTag[64] = "Not dolphin";

const char *GetDolphinTag() {
    return sDolphinTag;
}

static HostPlatform sPlatform = HOST_UNKNOWN;

HostPlatform getHostPlatform() {
    return sPlatform;
}

void InitHost() {
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
    case 0xDEAD:
        sPlatform = HOST_TEGRA;
        return;
    }

    sPlatform = HOST_UNKNOWN;
}