#pragma once

#include <Common.h>

typedef void (*GameEntryFunc)(void);

bool Apploader_loadAndRun(GameEntryFunc *gameEntry);
