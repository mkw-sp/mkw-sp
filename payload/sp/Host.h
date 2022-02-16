#pragma once

#include <Common.h>

typedef enum {
    HOST_UNKNOWN,
    HOST_REVOLUTION,
    HOST_CAFE,
    HOST_TEGRA,
    HOST_DOLPHIN_UNKNOWN,
    HOST_DOLPHIN,
} HostPlatform;

const char* GetDolphinTag();
HostPlatform getHostPlatform();

void InitHost();

inline bool CheckIfGeckoCodesEnabled() {
    return *(volatile u8 *)0x800018A8 == 0x94;
}