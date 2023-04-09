#pragma once

#include "game/system/Mii.h"

bool SaveManager_SaveGhostResult(void);

u32 SaveManager_SPLicenseCount(void);
s32 SaveManager_SPCurrentLicense(void);

u32 SaveManager_GetVanillaMode(void);

u32 SaveManager_GetFOV169(void);

u32 SaveManager_GetMapIcons(void);

u32 SaveManager_GetTAClass(void);

u32 SaveManager_GetTAGhostTagVisibility(void);

u32 SaveManager_GetTASolidGhosts(void);

u32 SaveManager_GetTAGhostSound(void);

MiiId SaveManager_GetSPLicenseMiiId(u32 license);

enum {
    VS_RULE_CLASS_100CC = 0x0,
    VS_RULE_CLASS_150CC = 0x1,
    VS_RULE_CLASS_MIRROR = 0x2,
    VS_RULE_CLASS_200CC = 0x3,
};

extern bool vsSpeedModIsEnabled;
