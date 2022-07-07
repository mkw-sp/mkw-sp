#include "eggSystem.h"

#include <game/item/ItemDirector.h>
#include <game/system/Console.h>
#include <game/system/RaceConfig.h>
#include <game/system/SaveManager.h>
#include <game/ui/SectionManager.h>
#include <sp/Commands.h>
#include <sp/Host.h>
#include <sp/IOSDolphin.h>
#include <sp/StringView.h>
#include <sp/keyboard/Keyboard.h>
#include <sp/storage/LogFile.h>
#include <stdio.h>
#include <stdlib.h>  // qsort
#include <string.h>  // memcpy

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

static s32 GetMyPlayerID(void) {
    if (s_raceConfig == NULL) {
        OSReport("&cNot in a race.\n");
        return -1;
    }
    s32 myPlayerId = s_raceConfig->raceScenario.localPlayerIds[0];
    if (myPlayerId < 0 || myPlayerId > 11) {
        OSReport(
                "&cCurrent player id is &4%i&c. Expected in range &4[0, 11]&c. Are you a "
                "spectator?\n",
                myPlayerId);
        return -1;
    }
    return myPlayerId;
}
bool sItemSticky = false;
int sStickyItem = 0;
int sStickyQty = 1;
static void ClearItem(s32 myPlayerId) {
    if (s_itemDirector != NULL) {
        s_itemDirector->mKartItems[myPlayerId].mCurrentItemKind = 20;
        s_itemDirector->mKartItems[myPlayerId].mCurrentItemQty = 0;
    }
    sItemSticky = false;
}
static void TrySetItem(s32 myPlayerId, s32 item, s32 qty) {
    if (s_itemDirector != NULL) {
        s_itemDirector->mKartItems[myPlayerId].mCurrentItemKind = item;
        s_itemDirector->mKartItems[myPlayerId].mCurrentItemQty = qty;
    }
}

sp_define_command("/i", "Spawn an item.", const char *tmp) {
    const s32 myPlayerId = GetMyPlayerID();
    if (myPlayerId < 0) {
        return;
    }

    // No args
    if (!strcmp(tmp, "/i")) {
        OSReport("&aClearing item\n");
        ClearItem(myPlayerId);
        return;
    }
    // Args
    if (!strncmp(tmp, "/i ", strlen("/i "))) {
        int item = 0;
        int qty = 1;
        if (!sscanf(tmp, "/i %d %d", &item, &qty)) {
            OSReport("&aUnknown arguments \"%s\". Usage: /i [item] [qty|0=STICKY]\n",
                    tmp + strlen("/i "));
            return;
        }
        if (qty <= 0) {
            qty = MAX(-qty, 1);
            sItemSticky = true;
            sStickyItem = item;
            sStickyQty = qty;
            OSReport("&aSpawning item %d (&2STICKY %dx&a)\n", item, qty);
        } else {
            sItemSticky = false;
            OSReport("&aSpawning item %d (&2%dx&a)\n", item, qty);
        }

        TrySetItem(myPlayerId, item, qty);
        return;
    }

    OSReport("&4Invalid command: \"%s\"", tmp);
}

sp_define_command("/example_command", "Example command", const char *tmp) {
    (void)tmp;

    if (!SaveManager_IsAvailable()) {
        OSReport("example_command: Failed to load Save Manager\n");
        return;
    }

    static const char *tagContent[4] = {
        "SP_TA_RULE_GHOST_TAG_CONTENT_NAME",
        "SP_TA_RULE_GHOST_TAG_CONTENT_TIME",
        "SP_TA_RULE_GHOST_TAG_CONTENT_TIME_NOLEADING",
        "SP_TA_RULE_GHOST_TAG_CONTENT_DATE",
    };
    const u32 rule = SaveManager_GetTAGhostTagContent();
    OSReport("example_command: taRuleGhostTagContent == %s\n", tagContent[rule & 3]);
}

sp_define_command("/instant_menu", "Toggle instant menu transitions", const char *tmp) {
    (void)tmp;

    if (!SaveManager_IsAvailable()) {
        OSReport("instant_menu: Failed to load Save Manager\n");
        return;
    }
    const bool menuTrans = !SaveManager_GetPageTransitions();
    SaveManager_SetPageTransitions(menuTrans);
    OSReport("instant_menu: Menu transition animations toggled %s\n", sOnOff[menuTrans]);
}

sp_define_command("/set", "Sets a .ini setting key-value", const char *tmp) {
    if (!SaveManager_IsAvailable()) {
        OSReport("set: Failed to load Save Manager\n");
        return;
    }
    char setting[64];
    char value[64];
    if (2 != sscanf(tmp, "/set %63s %63s", setting, value)) {
        OSReport("&a/set: Invalid arguments\n");
        return;
    }
    if (SaveManager_SPCurrentLicense()) {
        OSReport("&a/set: No license active\n");
        return;
    }
    SaveManager_SetSetting(setting, value);
}

sp_define_command("/section", "Transition to a certain game section", const char *tmp) {
    if (s_sectionManager == NULL) {
        OSReport("&aError: Section manager unavailable\n");
        return;
    }
    int nextSectionId = 0;
    if (!sscanf(tmp, "/section %i", &nextSectionId)) {
        OSReport("&aUsage /section <id>\n");
        return;
    }

    OSReport("&aSwitching to section %d\n", nextSectionId);

    if (!SaveManager_IsAvailable()) {
        OSReport("&aError: Save manager unavailable\n");
        return;
    }

    // Default to license 0
    SaveManager_SelectSPLicense(0);
    // TODO create base license
    SaveManager_SelectLicense(0);

    SectionManager_setNextSection(s_sectionManager, __builtin_abs(nextSectionId),
            nextSectionId < 0 ? PAGE_ANIMATION_PREV : PAGE_ANIMATION_NEXT);
    SectionManager_startChangeSection(s_sectionManager, 5, 0xff);
}

sp_define_command("/dolphin_test", "Test /dev/dolphin driver", const char *tmp) {
    (void)tmp;

    DolphinTest();
}

static void my_lineCallback(const char *buf, size_t len) {
    StringView view = (StringView){ .s = buf, .len = len };
    const char *tmp = sv_as_cstr(view, 64);

    OSReport("[SP] Line submitted: %s\n", tmp);

    const Command *it = Commands_match(tmp);
    if (it == NULL) {
        OSReport("&aUnknown command \"%s\"\n", tmp);
        return;
    }
    if (it->onBegin == NULL) {
        OSReport("&aCommand not implemented \"%s\"\n", tmp);
        return;
    }
    it->onBegin(tmp);
}

// IOS KBD module is not supported on this platform
static bool sConsoleInputUnavailable = false;

void my_onBeginFrame(void *UNUSED(system)) {
    LogFile_Flush();

    if (sItemSticky) {
        const s32 myPlayerId = GetMyPlayerID();
        if (myPlayerId >= 0) {
            TrySetItem(myPlayerId, sStickyItem, sStickyQty);
        }
    }
    if (sConsoleInputUnavailable)
        return;

    if (!SP_IsConsoleInputInit()) {
        if (SP_InitConsoleInput()) {
            Commands_init();
            SP_SetLineCallback(my_lineCallback);
        } else {
            // Do not try again
            sConsoleInputUnavailable = true;
            return;
        }
    }

    SP_ProcessConsoleInput();
    Console_calc();
}

PATCH_B(EGG_ConfigurationData_onBeginFrame, my_onBeginFrame);
PATCH_B(EGG_ProcessMeter_draw + 0xa4, Console_draw);
