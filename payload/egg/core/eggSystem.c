#include <Common.h>
#include <game/item/ItemDirector.h>
#include <game/system/Console.h>
#include <game/system/RaceConfig.h>
#include <game/system/SaveManager.h>
#include <game/ui/SectionManager.h>
#include <revolution.h>  // OSReport
#include <sp/Host.h>
#include <sp/IOSDolphin.h>
#include <sp/Keyboard.h>
#include <stdio.h>
#include <string.h>  // memcpy

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

// Matches "/command", "/command arg", but not "/command2"
static bool StringStartsWithCommand(const char *line, const char *cmd) {
    const size_t cmdLen = strlen(cmd);
    if (strncmp(line, cmd, cmdLen)) {
        return false;
    }
    return line[cmdLen] == '\0' || line[cmdLen] == ' ';
}

static void SlashICommand(const char *tmp) {
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

static void my_lineCallback(const char *buf, size_t len) {
    // Demo functions
    char tmp[64];
    if (len > sizeof(tmp) - 1)
        len = sizeof(tmp) - 1;
    memcpy(tmp, buf, len);
    tmp[len] = '\0';

    OSReport("[SP] Line submitted: %s\n", tmp);

    if (StringStartsWithCommand(tmp, "/example_command")) {
        if (s_saveManager == NULL) {
            OSReport("example_command: Failed to load Save Manager\n");
        } else {
            static const char *tagContent[4] = {
                "SP_TA_RULE_GHOST_TAG_CONTENT_NAME",
                "SP_TA_RULE_GHOST_TAG_CONTENT_TIME",
                "SP_TA_RULE_GHOST_TAG_CONTENT_TIME_NOLEADING",
                "SP_TA_RULE_GHOST_TAG_CONTENT_DATE",
            };
            const u32 rule =
                    SaveManager_getSetting(s_saveManager, kSetting_TaRuleGhostTagContent);
            OSReport("example_command: taRuleGhostTagContent == %s\n",
                    tagContent[rule & 3]);
        }
        return;
    }

    if (StringStartsWithCommand(tmp, "/instant_menu")) {
        if (s_saveManager == NULL) {
            OSReport("instant_menu: Failed to load Save Manager\n");
        } else {
            bool menuTrans =
                    !SaveManager_getSetting(s_saveManager, kSetting_PageTransitions);
            SaveManager_setSetting(s_saveManager, kSetting_PageTransitions, menuTrans);
            OSReport("instant_menu: Menu transition animations toggled %s\n",
                    sOnOff[menuTrans]);
        }
        return;
    }

    if (StringStartsWithCommand(tmp, "/set")) {
        char setting[64];
        char value[64];
        if (2 != sscanf(tmp, "/set %s %s", setting, value)) {
            OSReport("&a/set: Invalid arguments\n");
            return;
        }
        if (s_saveManager->spCurrentLicense < 0) {
            OSReport("&a/set: No license active\n");
            return;
        }
        ClientSettings_set(&s_saveManager->spLicenses[s_saveManager->spCurrentLicense]->cfg, setting, value);
        return;
    }

    if (StringStartsWithCommand(tmp, "/dolphin_test")) {
        DolphinTest();
        return;
    }

    if (StringStartsWithCommand(tmp, "/i")) {
        SlashICommand(tmp);
        return;
    }

    if (StringStartsWithCommand(tmp, "/section")) {
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

        if (s_saveManager == NULL) {
            OSReport("&aError: Save manager unavailable\n");
            return;
        }

        // Default to license 0
        s_saveManager->spCurrentLicense = 0;
        SaveManager_selectLicense(s_saveManager, 0);

        SectionManager_setNextSection(s_sectionManager, __builtin_abs(nextSectionId),
                nextSectionId < 0 ? PAGE_ANIMATION_PREV : PAGE_ANIMATION_NEXT);
        SectionManager_startChangeSection(s_sectionManager, 5, 0xff);
        return;
    }

    OSReport("&aUnknown command \"%s\"\n", tmp);
}

// IOS KBD module is not supported on this platform
static bool sConsoleInputUnavailable = false;

void my_onBeginFrame(void *UNUSED(system)) {
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

void EGG_ProcessMeter_draw(void *);

PATCH_B(EGG_ProcessMeter_draw + 0xa4, Console_draw);
