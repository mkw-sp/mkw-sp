#pragma once

#define S(x) #x
#define S2(x) S(x)

#ifndef GIT_HASH
#define _GIT_HASH "<GIT ?>"
#else
#define _GIT_HASH S2(GIT_HASH)
#endif

#ifdef GIT_CHANGED_FILES
#define _GIT_CHANGED_FILES S2(GIT_CHANGED_FILES)
#else
#define _GIT_CHANGED_FILES ""
#endif

#define _HAS_GIT_CHANGES (_GIT_CHANGED_FILES[0] != '\0')

#ifdef DEBUG
#define BUILD_TYPE_STR "Debug " _GIT_HASH
#else
#define BUILD_TYPE_STR "0.1.4 (Release) REV " _GIT_HASH
#endif

#ifdef __VERSION__

#ifdef __clang__
#define CC_STR __VERSION__
#else
#define CC_STR "GCC " __VERSION__
#endif

#else
#define CC_STR "Unknown"
#endif

#define SP_VERSION BUILD_TYPE_STR " (Built " __DATE__ " at " __TIME__ ", " CC_STR ")"
