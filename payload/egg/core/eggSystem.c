#include <Common.h>

#include <sp/Keyboard.h>

#include <revolution.h>  // OSReport
#include <string.h>      // memcpy

#include <game/system/SaveManager.h>

extern void EGG_ConfigurationData_onBeginFrame(void *system);

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
    }
}

void my_onBeginFrame(void *UNUSED(system)) {
    if (!SP_IsConsoleInputInit()) {
        SP_InitConsoleInput();
        SP_SetLineCallback(my_lineCallback);

        assert(SP_IsConsoleInputInit() && "Failed to initialize console");
    }

    SP_ProcessConsoleInput();
}

PATCH_B(EGG_ConfigurationData_onBeginFrame, my_onBeginFrame);