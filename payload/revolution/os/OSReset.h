#pragma once

#include <Common.h>

enum {
    OS_RESET_RESTART = 0,
};

void OSShutdownSystem(void);
void OSRestart(u32 resetCode);
void OSReturnToMenu(void);
