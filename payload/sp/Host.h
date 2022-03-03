#pragma once

#include <Common.h>

typedef enum {
    //! This is either a bug in the platform detection code,
    //! or a novel platform (unlikely).
    //!
    HOST_UNKNOWN,

    //! A physical Wii console.
    //!
    HOST_REVOLUTION,

    //! A physical WiiU console running in vWii mode.
    //!
    HOST_CAFE,

    //! Old Dolphin (< 5.0-11186 [Nov 10, 2019 df32e3f])
    //!
    HOST_DOLPHIN_UNKNOWN,

    //! Modern Dolphin (>= 5.0-11186 [Nov 10, 2019 df32e3f])
    //! - The specific version is available with `Host_GetDolphinTag()`
    //!
    HOST_DOLPHIN,
} HostPlatform;

void Host_Init(void);

// Cached
const char *Host_GetDolphinTag(void);
HostPlatform Host_GetPlatform(void);

// Not cached
bool Host_IsGeckoEnabled(void);
const char *Host_GetRegionString(void);

typedef void PrintfFunction(const char *, ...);

// Example output:
//     --------------------------------
//     MKW-SP v0.1.4 (Release) REV 15610c0
//     Region: PAL, System: Dolphin 5.0-15993
//     Built Mar  2 2022 at 23:22:40, GCC 10.2.0
//     --------------------------------
void Host_PrintMkwSpInfo(PrintfFunction *func);
