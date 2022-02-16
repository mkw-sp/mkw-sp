#include <Common.h>
#include <sp/Host.h>
#include <sp/Version.h>

extern void Exception_Printf_(const char *, ...);
extern void Exception_PrintCtx();

static void line1(const char *UNUSED(str), u32 frame) {
    const char *region = "?";

    switch (REGION) {
    case REGION_P:
        region = "PAL";
        break;
    case REGION_E:
        region = "NTSC-U";
        break;
    case REGION_J:
        region = "NTSC-J";
        break;
    case REGION_K:
        region = "NTSC-K";
        break;
    }

    Exception_Printf_("--------------------------------\n");
    Exception_Printf_("MKW-SP v" BUILD_TYPE_STR "\nRegion: %s, System: ", region);

    switch (getHostPlatform()) {
    case HOST_REVOLUTION:
        Exception_Printf_("Wii");
        break;
    case HOST_CAFE:
        Exception_Printf_("WiiU");
        break;
    case HOST_TEGRA:
        Exception_Printf_("Tegra");
        break;
    case HOST_DOLPHIN_UNKNOWN:
        Exception_Printf_("Dolphin (Unsupported)");
        break;
    case HOST_DOLPHIN:
        Exception_Printf_("Dolphin %s", GetDolphinTag());
        break;
    case HOST_UNKNOWN:
        Exception_Printf_("Unknown host");
        break;
    }

    if (CheckIfGeckoCodesEnabled()) {
        Exception_Printf_("*");
    }

    Exception_Printf_("\n");

    Exception_Printf_("Built " __DATE__ " at " __TIME__ ", " CC_STR "\n");

    if (_HAS_GIT_CHANGES) {
        Exception_Printf_("Changed files: " _GIT_CHANGED_FILES "\n");
    }

    Exception_Printf_("--------------------------------\n");

    Exception_Printf_("******** EXCEPTION OCCURRED! ********\nFrameMemory:%XH\n", frame);
}
static void line2(void) {
    return Exception_Printf_("--------------------------------\n");
}

PATCH_BL(Exception_PrintCtx + 0x48, line1);
PATCH_BL(Exception_PrintCtx + 0x84, line2);
