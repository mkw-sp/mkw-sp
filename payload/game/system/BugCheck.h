#pragma once

#include <Common.h>

// Forcibly pull the system down in a controlled manner.
void SpBugCheck(const char *file, const char *description);
