#pragma once

#include "game/system/Mii.h"

bool SaveManager_IsAvailable(void);

bool SaveManager_SaveGhostResult(void);

void SaveManager_EraseLicense(u32 licenseId);
void SaveManager_SelectLicense(u32 licenseId);

u32 SaveManager_SPLicenseCount(void);
void SaveManager_EraseSPLicense(void);
void SaveManager_CreateSPLicense(const MiiId *miiId);
s32 SaveManager_SPCurrentLicense(void);
void SaveManager_SelectSPLicense(u32 licenseId);
void SaveManager_UnselectSPLicense(void);

void SaveManager_SetSetting(const char *key, const char *value);

u32 SaveManager_GetVanillaMode(void);

u32 SaveManager_GetFOV169(void);

u32 SaveManager_GetMapIcons(void);

u32 SaveManager_GetInputDisplay(void);

u32 SaveManager_GetTAClass(void);

u32 SaveManager_GetTAGhostTagVisibility(void);

u32 SaveManager_GetTAGhostTagContent(void);

u32 SaveManager_GetTASolidGhosts(void);

u32 SaveManager_GetTAGhostSound(void);

u32 SaveManager_GetHUDLabels(void);

u32 SaveManager_GetPageTransitions(void);
void SaveManager_SetPageTransitions(u32 value);

void SaveManager_SetMiiId(const MiiId *miiId);

MiiId SaveManager_GetSPLicenseMiiId(u32 license);

const u8 *SaveManager_CourseSHA1(u32 courseId);

bool SaveManager_getItemWheel();

enum {
    VS_RULE_CLASS_100CC = 0x0,
    VS_RULE_CLASS_150CC = 0x1,
    VS_RULE_CLASS_MIRROR = 0x2,
    VS_RULE_CLASS_200CC = 0x3,
};

extern bool vsSpeedModIsEnabled;
