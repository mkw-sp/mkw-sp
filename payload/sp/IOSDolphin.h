#pragma once

#include <Common.h>

// Negative is invalid
typedef s32 IOSDolphin;

// Return if the operation succeeded
IOSDolphin IOSDolphin_Open();
static inline bool IOSDolphin_IsOpen(IOSDolphin dolphin) {
    return dolphin >= 0;
}
void IOSDolphin_Close(IOSDolphin dolphin);

typedef struct {
    u32 milliseconds;
    bool hasValue;
} IOSDolphin_SystemTimeQuery;

// Ensure IOSDolphin_IsOpen before calling the following APIs
IOSDolphin_SystemTimeQuery IOSDolphin_GetSystemTime(IOSDolphin dolphin);

typedef struct {
    // Will always be null-terminated
    char version[64];
    bool hasValue;
} IOSDolphin_VersionQuery;

IOSDolphin_VersionQuery IOSDolphin_GetVersion(IOSDolphin dolphin);

typedef struct {
    u32 ticksPerSecond;
    bool hasValue;
} IOSDolphin_CPUSpeedQuery;

IOSDolphin_CPUSpeedQuery IOSDolphin_GetCPUSpeed(IOSDolphin dolphin);

typedef struct {
    u32 emulationSpeedPercent;  // [0-200]
    bool hasValue;
} IOSDolphin_SpeedLimitQuery;

IOSDolphin_SpeedLimitQuery IOSDolphin_GetSpeedLimit(IOSDolphin dolphin);

// Return if success
bool IOSDolphin_SetSpeedLimit(IOSDolphin dolphin, u32 emulationSpeedPercent);

typedef struct {
    // The code is 3 characters long (dolphin default: 'DOL')
    char code[4];
    bool hasValue;
} IOSDolphin_RealProductCodeQuery;

IOSDolphin_RealProductCodeQuery IOSDolphin_GetRealProductCode(IOSDolphin dolphin);
