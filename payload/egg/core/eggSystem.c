#include "eggSystem.h"

#include <game/system/Console.h>
#include <sp/Commands.h>
#include <sp/ItemCommand.h>
#include <sp/keyboard/Keyboard.h>

// IOS KBD module is not supported on this platform
static bool sConsoleInputUnavailable = false;

void my_onBeginFrame(void * /* system */) {
    Item_beginFrame();
    if (sConsoleInputUnavailable) {
        return;
    }

    if (!SP_IsConsoleInputInit()) {
        if (SP_InitConsoleInput()) {
            Commands_init();
            SP_SetLineCallback(Commands_lineCallback);
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
