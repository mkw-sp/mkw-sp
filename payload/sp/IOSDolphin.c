#include "IOSDolphin.h"
#include <revolution.h>
#include <revolution/ios.h>
#include <stdalign.h>
#include <string.h>

// Debug logging
#define IOSDOLPHIN_LOG(...) OSReport(__VA_ARGS__)

typedef enum {
    // Merged in df32e3f Nov 10, 2019
    //
    // Dolphin 5.0-11186
    kDevDolphinIoctl_GetSystemTime = 1,  // Vector IN() OUT(u32)
    kDevDolphinIoctl_GetVersion = 2,     // Vector IN() OUT(char[])
    kDevDolphinIoctl_GetSpeedLimit = 3,  // Vector IN() OUT(u32)
    kDevDolphinIoctl_SetSpeedLimit = 4,  // Vector IN(u32) OUT()
    kDevDolphinIoctl_GetCPUSpeed = 5,    // Vector IN() OUT(u32)

    // Merged in 393ce52 May 22, 2020
    //
    // Dolphin 5.0-12058
    kDevDolphinIoctl_GetRealProductCode = 6,  // Vector IN() OUT(char[])
} DevDolphinIoctl;

IOSDolphin IOSDolphin_Open() {
    return IOS_Open("/dev/dolphin", 0);
}

void IOSDolphin_Close(IOSDolphin dolphin) {
    if (dolphin >= 0) {
        IOS_Close(dolphin);
    }
}

IOSDolphin_SystemTimeQuery IOSDolphin_GetSystemTime(IOSDolphin dolphin) {
    assert(IOSDolphin_IsOpen(dolphin));

    IOSDolphin_SystemTimeQuery query = (IOSDolphin_SystemTimeQuery){
        .milliseconds = 0,
        .hasValue = false,
    };

    alignas(32) IOVector vec = (IOVector){
        .data = &query.milliseconds,
        .size = sizeof(query.milliseconds),
    };

    const s32 err = IOS_Ioctlv(dolphin, kDevDolphinIoctl_GetSystemTime, /*inputCount*/ 0,
            /*outputCount*/ 1, &vec);
    query.hasValue = err == IPC_OK;
    if (!query.hasValue) {
        IOSDOLPHIN_LOG(
                "[IOSDolphin] GetSystemTime: Failed with IPC error code %i\n", err);
    }

    return query;
}

IOSDolphin_VersionQuery IOSDolphin_GetVersion(IOSDolphin dolphin) {
    assert(IOSDolphin_IsOpen(dolphin));

    IOSDolphin_VersionQuery query;
    memset(&query.version, 0, sizeof(query.version));
    query.hasValue = false;

    alignas(32) IOVector vec = (IOVector){
        .data = &query.version,
        .size = sizeof(query.version),
    };

    const s32 err = IOS_Ioctlv(dolphin, kDevDolphinIoctl_GetVersion, /*inputCount*/ 0,
            /*outputCount*/ 1, &vec);
    query.hasValue = err == IPC_OK;

    if (!query.hasValue) {
        IOSDOLPHIN_LOG("[IOSDolphin] GetVersion: Failed with IPC error code %i\n", err);
        return query;
    }

    if (query.version[sizeof(query.version) - 1] != '\0') {
        query.version[sizeof(query.version) - 1] = '\0';
        // This should never happen.
        IOSDOLPHIN_LOG(
                "[IOSDolphin] GetVersion: Dolphin returned invalid version string: %s\n",
                query.version);
        query.hasValue = false;
    }

    return query;
}

IOSDolphin_CPUSpeedQuery IOSDolphin_GetCPUSpeed(IOSDolphin dolphin) {
    assert(IOSDolphin_IsOpen(dolphin));

    IOSDolphin_CPUSpeedQuery query = (IOSDolphin_CPUSpeedQuery){
        .ticksPerSecond = 0,
        .hasValue = false,
    };

    alignas(32) IOVector vec = (IOVector){
        .data = &query.ticksPerSecond,
        .size = sizeof(query.ticksPerSecond),
    };

    const s32 err = IOS_Ioctlv(dolphin, kDevDolphinIoctl_GetCPUSpeed, /*inputCount*/ 0,
            /*outputCount*/ 1, &vec);
    query.hasValue = err == IPC_OK;

    if (!query.hasValue) {
        IOSDOLPHIN_LOG("[IOSDolphin] GetCPUSpeed: Failed with IPC error code %i\n", err);
    }

    return query;
}

IOSDolphin_SpeedLimitQuery IOSDolphin_GetSpeedLimit(IOSDolphin dolphin) {
    assert(IOSDolphin_IsOpen(dolphin));

    IOSDolphin_SpeedLimitQuery query = (IOSDolphin_SpeedLimitQuery){
        .emulationSpeedPercent = 0,
        .hasValue = false,
    };

    alignas(32) IOVector vec = (IOVector){
        .data = &query.emulationSpeedPercent,
        .size = sizeof(query.emulationSpeedPercent),
    };

    const s32 err = IOS_Ioctlv(dolphin, kDevDolphinIoctl_GetSpeedLimit, /*inputCount*/ 0,
            /*outputCount*/ 1, &vec);
    query.hasValue = err == IPC_OK;

    if (!query.hasValue) {
        IOSDOLPHIN_LOG(
                "[IOSDolphin] GetSpeedLimit: Failed with IPC error code %i\n", err);
    }

    return query;
}

bool IOSDolphin_SetSpeedLimit(IOSDolphin dolphin, u32 emulationSpeedPercent) {
    assert(IOSDolphin_IsOpen(dolphin));

    alignas(32) IOVector vec = (IOVector){
        .data = &emulationSpeedPercent,
        .size = sizeof(emulationSpeedPercent),
    };

    const s32 err = IOS_Ioctlv(dolphin, kDevDolphinIoctl_SetSpeedLimit, /*inputCount*/ 1,
            /*outputCount*/ 0, &vec);
    if (err != IPC_OK) {
        IOSDOLPHIN_LOG(
                "[IOSDolphin] SetSpeedLimit: Failed with IPC error code %i\n", err);
        return false;
    }

    return true;
}

IOSDolphin_RealProductCodeQuery IOSDolphin_GetRealProductCode(IOSDolphin dolphin) {
    assert(IOSDolphin_IsOpen(dolphin));

    IOSDolphin_RealProductCodeQuery query;
    memset(&query.code, 0, sizeof(query.code));
    query.hasValue = false;

    alignas(32) IOVector vec = (IOVector){
        .data = &query.code,
        .size = sizeof(query.code),
    };

    const s32 err =
            IOS_Ioctlv(dolphin, kDevDolphinIoctl_GetRealProductCode, /*inputCount*/ 0,
                    /*outputCount*/ 1, &vec);
    query.hasValue = err == IPC_OK;

    if (!query.hasValue) {
        IOSDOLPHIN_LOG(
                "[IOSDolphin] GetRealProductCode: Failed with IPC error code %i\n", err);
        return query;
    }

    if (query.code[sizeof(query.code) - 1] != '\0') {
        query.code[sizeof(query.code) - 1] = '\0';
        // This should never happen.
        IOSDOLPHIN_LOG(
                "[IOSDolphin] GetRealProductCode: Dolphin returned invalid version "
                "string: %s\n",
                query.code);
        query.hasValue = false;
    }

    return query;
}
