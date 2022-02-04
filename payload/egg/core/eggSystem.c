#include <Common.h>

#include <sp/Keyboard.h>

#include <revolution.h>  // OSReport
#include <string.h>      // memcpy

#include <game/system/SaveManager.h>
#include <game/ui/Page.h>

extern void EGG_ConfigurationData_onBeginFrame(void *system);

static const char* sOnOff[2] = {
    "OFF",
    "ON"
};

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
        bool menuTrans = !Page_getTransitionsEnabled();
        Page_setTransitionsEnabled(menuTrans);
        OSReport("instant_menu: Menu transition animations toggled %s\n", sOnOff[menuTrans]);
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