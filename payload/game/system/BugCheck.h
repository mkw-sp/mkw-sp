#pragma once

#include <Common.h>

// Forcibly pull the system down in a controlled manner.
//
void SpBugCheck(const char *file, const char *description);
void SpBugCheckFmt(const char *file, const char *desc, ...);

// Set a flag. The user must then construct a scene SCENE_ID_SPFATAL. Recoverable.
//
void SpBugCheckAsync(const char *file, const char *description);
const wchar_t *GetFormattedBugCheck(void);

// e.g. `boardwalk_course.szs`
void SetContextForBugCheck(const char *context);
