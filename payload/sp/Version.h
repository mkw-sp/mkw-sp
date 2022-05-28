#pragma once

#ifdef __VERSION__

#ifdef __clang__
#define CC_STR __VERSION__
#else
#define CC_STR "GCC " __VERSION__
#endif

#else
#define CC_STR "Unknown"
#endif
