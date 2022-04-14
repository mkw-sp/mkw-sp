#pragma once

#include <sp/settings/ClientSettings.h>

// Bit fields
typedef struct {
    u32 driftMode : 1;
    u32 settingHudLabels : 1;
    u32 setting169Fov : 1;
    u32 settingMapIcons : 1;
    u32 taRuleClass : 1;
    u32 taRuleGhostSorting : 2;
    u32 taRuleGhostTagVisibility : 2;
    u32 taRuleGhostTagContent : 2;
    u32 taRuleSolidGhosts : 2;
    u32 settingPageTransitions : 1;
    u32 settingRaceInputDisplay : 3;
    u32 taRuleGhostSound : 2;
} SpSettingBitField;
