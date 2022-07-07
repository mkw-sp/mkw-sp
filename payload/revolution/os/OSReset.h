#pragma once

#include <Common.h>

enum {
    OS_RESET_RESTART = 0,
};

// Not a public function
void __OSShutdownDevices(u32 r3);

void OSShutdownSystem(void);
void OSRestart(u32 resetCode);
void OSReturnToMenu(void);
