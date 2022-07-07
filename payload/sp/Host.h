#pragma once

#include <Common.h>

// Keep this value in sync with the value in the file 'mkw-sp/protobuf/UpdateRequestMessage.options'
#define HOST_PLATFORM_BUFFER_SIZE (31 + 1)

// clang-format off
typedef enum {
    //! This is either a bug in the platform detection code,
    //! or a novel platform (unlikely).
    //!
    HOST_UNKNOWN,

    __HOST_CONSOLE_BEGIN,

        //! A physical Wii console. (RVL-101)
        //!
        HOST_REVOLUTION,

        //! A physical Wii console. (RVL-201)
        //!
        HOST_WII_MINI,

        //! A physical WiiU console running in vWii mode.
        //!
        HOST_CAFE,

    __HOST_CONSOLE_END,

    __HOST_DOLPHIN_BEGIN,

        //! Ancient Dolphin (< Dec 16, 2009)
        //!
        HOST_DOLPHIN_PREHISTORIC,

        //! Old Dolphin (< 5.0-11186 [Nov 10, 2019 df32e3f])
        //!
        HOST_DOLPHIN_UNKNOWN,

        //! Modern Dolphin (>= 5.0-11186 [Nov 10, 2019 df32e3f])
        //! - The specific version is available with `Host_GetDolphinTag()`
        //!
        HOST_DOLPHIN,

    __HOST_DOLPHIN_END,

    __HOST_PC_BEGIN,

        //! Windows
        HOST_WINDOWS,

        //! MacOS/iOS/iPadOS/tvOS
        HOST_APPLE,

        //! Linux
        HOST_LINUX,

    __HOST_PC_END,
} HostPlatform;
// clang-format on

static inline bool HostPlatform_IsKnown(HostPlatform platform) {
    return platform != HOST_UNKNOWN;
}
static inline bool HostPlatform_IsConsole(HostPlatform platform) {
    return platform > __HOST_CONSOLE_BEGIN && platform < __HOST_CONSOLE_END;
}
static inline bool HostPlatform_IsDolphin(HostPlatform platform) {
    return platform > __HOST_DOLPHIN_BEGIN && platform < __HOST_DOLPHIN_END;
}
static inline bool HostPlatform_IsPC(HostPlatform platform) {
    return platform > __HOST_PC_BEGIN && platform < __HOST_PC_END;
}

void Host_Init(void);

// Cached
const char *Host_GetDolphinTag(void);
HostPlatform Host_GetPlatform(void);
const char* Host_GetPlatformString(void);

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

#ifndef PLATFORM_EMULATOR
typedef struct {
    u32 _924;
    u32 ECID_H;
    u32 ECID_M;
    u32 ECID_L;
} RawCPU_ID;

extern const RawCPU_ID sCpuId_PrehistoricDolphin;
extern const RawCPU_ID sCpuId_Dolphin;

RawCPU_ID Host_GetCPUID(void);
void Host_SetCPUID(RawCPU_ID id);
#endif
