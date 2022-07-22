#pragma once

#include <Common.h>

enum {
    OS_RESET_RESTART = 0,
};

// These functions aren't public
void __OSShutdownDevices(u32 r3);
void __OSReturnToMenuForError(void);

void OSShutdownSystem(void);
void OSRestart(u32 resetCode);
void OSReturnToMenu(void);
