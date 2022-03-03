#include <Common.h>

#include <sp/Keyboard.h>

#include <revolution.h>  // OSReport
#include <string.h>      // memcpy

#include <game/system/SaveManager.h>

#include <sp/Host.h>
#include <sp/IOSDolphin.h>

extern void EGG_ConfigurationData_onBeginFrame(void *system);

static const char *sOnOff[2] = { "OFF", "ON" };
static const char *fmtBool(bool b) {
    return sOnOff[!!b];
}

static void emuspeed(IOSDolphin dolphin, u32 *out) {
    IOSDolphin_SpeedLimitQuery emuspeed = IOSDolphin_GetSpeedLimit(dolphin);
    OSReport("emuspeed: percent=%u,hasValue=%s\n", emuspeed.emulationSpeedPercent,
            fmtBool(emuspeed.hasValue));

    if (out != NULL && emuspeed.hasValue)
        *out = emuspeed.emulationSpeedPercent;
}
static void set_emuspeed(IOSDolphin dolphin, u32 speed) {
    const bool res = IOSDolphin_SetSpeedLimit(dolphin, speed);
    OSReport("set_emuspeed: success=%s\n", fmtBool(res));
}

static void DolphinTest() {
    // Open a IOS handle for this test
    IOSDolphin dolphin = IOSDolphin_Open();
    if (!IOSDolphin_IsOpen(dolphin)) {
        OSReport("Failed to open IOSDolphin.\n");
        return;
    }

    {
        IOSDolphin_SystemTimeQuery systime = IOSDolphin_GetSystemTime(dolphin);
        OSReport("systime: milliseconds=%u,hasValue=%s\n", systime.milliseconds,
                fmtBool(systime.hasValue));
    }
    {
        IOSDolphin_VersionQuery version = IOSDolphin_GetVersion(dolphin);
        OSReport("version: version=%s,hasValue=%s\n", version.version,
                fmtBool(version.hasValue));
    }
    {
        IOSDolphin_CPUSpeedQuery cpuspeed = IOSDolphin_GetCPUSpeed(dolphin);
        OSReport("cpuspeed: ticksPerSecond=%u,hasValue=%s\n", cpuspeed.ticksPerSecond,
                fmtBool(cpuspeed.hasValue));
    }
    {
        // Test GetSpeedLimit, SetSpeedLimit
        u32 old_emu_speed = 0;
        emuspeed(dolphin, &old_emu_speed);
        if (old_emu_speed) {
            set_emuspeed(dolphin, 50);
            // Report changed speed
            emuspeed(dolphin, NULL);
            // Reset speed
            set_emuspeed(dolphin, old_emu_speed);
            // Report changed speed
            emuspeed(dolphin, NULL);
        } else {
            OSReport("set_emuspeed: skipping\n");
        }
    }
    {
        IOSDolphin_RealProductCodeQuery prod = IOSDolphin_GetRealProductCode(dolphin);
        OSReport("prod: code=%s,hasValue=%s\n", prod.code, fmtBool(prod.hasValue));
    }
    IOSDolphin_Close(dolphin);
}

static void my_lineCallback(const char *buf, size_t len) {
    // Demo functions
    char tmp[64];
    if (len > sizeof(tmp) - 1)
        len = sizeof(tmp) - 1;
    memcpy(tmp, buf, len);
    tmp[len] = '\0';

    OSReport("[SP] Line submitted: %s\n", tmp);

    if (!strcmp(tmp, "/example_command")) {
        if (s_saveManager == NULL) {
            OSReport("example_command: Failed to load Save Manager\n");
        } else {
            static const char *tagContent[4] = {
                "SP_TA_RULE_GHOST_TAG_CONTENT_NAME",
                "SP_TA_RULE_GHOST_TAG_CONTENT_TIME",
                "SP_TA_RULE_GHOST_TAG_CONTENT_TIME_NOLEADING",
                "SP_TA_RULE_GHOST_TAG_CONTENT_DATE",
            };
            OSReport("example_command: taRuleGhostTagContent == %s\n",
                    tagContent[SaveManager_getTaRuleGhostTagContent(s_saveManager) & 3]);
        }
        return;
    }

    if (!strcmp(tmp, "/instant_menu")) {
        if (s_saveManager == NULL) {
            OSReport("instant_menu: Failed to load Save Manager\n");
        } else {
            bool menuTrans = !SaveManager_getSettingPageTransitions(s_saveManager);
            SaveManager_setSettingPageTransitions(s_saveManager, menuTrans);
            OSReport("instant_menu: Menu transition animations toggled %s\n",
                    sOnOff[menuTrans]);
        }
        return;
    }

    if (!strcmp(tmp, "/dolphin_test")) {
        DolphinTest();
        return;
    }
}

// IOS KBD module is not supported on this platform
static bool sConsoleInputUnavailable = false;

void my_onBeginFrame(void *UNUSED(system)) {
    if (sConsoleInputUnavailable)
        return;

    if (!SP_IsConsoleInputInit()) {
        if (SP_InitConsoleInput()) {
            SP_SetLineCallback(my_lineCallback);
        } else {
            // Do not try again
            sConsoleInputUnavailable = true;
            return;
        }
    }

    SP_ProcessConsoleInput();
}

PATCH_B(EGG_ConfigurationData_onBeginFrame, my_onBeginFrame);
