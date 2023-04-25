#pragma once

#include "game/system/Mii.h"

s32 SaveManager_SPCurrentLicense(void);

u32 SaveManager_GetVanillaMode(void);

u32 SaveManager_GetFOV169(void);

u32 SaveManager_GetMapIcons(void);

enum {
    VS_RULE_CLASS_100CC = 0x0,
    VS_RULE_CLASS_150CC = 0x1,
    VS_RULE_CLASS_MIRROR = 0x2,
    VS_RULE_CLASS_200CC = 0x3,
};

extern bool vsSpeedModIsEnabled;
