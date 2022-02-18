#include "eggColorFader.h"
#include <revolution.h>
#include <sp/IOSDolphin.h>

static IOSDolphin sDolphin = -1;
static bool sDolphinUnavailable = false;

static void Init() {
    if (sDolphin < 0)
        sDolphin = IOSDolphin_Open();

    if (sDolphin < 0)
        sDolphinUnavailable = true;
}

static bool SetEmuSpeed(u32 percent) {
    if (sDolphin >= 0) {
        OSReport("[SPEED] Set speed to %u\n", percent);
        return IOSDolphin_SetSpeedLimit(sDolphin, percent);
    }
    return false;
}

static u32 GetSpeedLimit() {
    if (sDolphin < 0)
        return 0;

    IOSDolphin_SpeedLimitQuery q = IOSDolphin_GetSpeedLimit(sDolphin);
    if (!q.hasValue)
        return 0;

    return q.emulationSpeedPercent;
}

static u32 sEmuSpeedStack[8];
static s32 sEmuSpeedStackSize = 0;

static void PushEmuSpeed(u32 percent) {
    if (sEmuSpeedStackSize == 8) {
        OSReport("[SPEED] Max depth reached\n");
        return;
    }

    const u32 old_limit = GetSpeedLimit();
    if (old_limit == 0) {
        OSReport("[SPEED] Failed to acquire current speed\n");
        return;
    }

    if (SetEmuSpeed(percent)) {
        sEmuSpeedStack[sEmuSpeedStackSize++] = old_limit;
    } else {
        OSReport("[SPEED] Failed to set speedlimit\n");
    }
}

static void PopEmuSpeed() {
    if (sEmuSpeedStackSize > 0) {
        SetEmuSpeed(sEmuSpeedStack[--sEmuSpeedStackSize]);
    }
}

static bool post_fadeIn(bool changed) {
    if (!sDolphinUnavailable) {
        Init();

        if (sDolphin >= 0 && changed)
            PopEmuSpeed();
    }

    return changed;
}

static bool post_fadeOut(bool changed) {
    if (!sDolphinUnavailable) {
        Init();
        if (sDolphin >= 0 && changed)
            PushEmuSpeed(400);  // Unlimited
    }

    return changed;
}

PATCH_B(EGG_ColorFader_fadeIn + 0x24, post_fadeIn);
PATCH_B(EGG_ColorFader_fadeOut + 0x28, post_fadeOut);
