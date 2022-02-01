#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x1ec - 0x000];
    u8 liveViewWatchedPlayerId;
    u8 _1ed[0x1f0 - 0x1ed];
} DriverManager;
static_assert(sizeof(DriverManager) == 0x1f0);

extern DriverManager *s_driverManager;

DriverManager *DriverManager_createInstance(void);
