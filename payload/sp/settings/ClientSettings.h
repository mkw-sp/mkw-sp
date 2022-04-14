#pragma once

#include <Common.h>
#include <sp/StringView.h>
#include <sp/settings/BaseSettings.h>
#include <stdio.h>

// Legacy enums
enum {
    SP_DRIFT_MODE_MANUAL = 0x0,
    SP_DRIFT_MODE_AUTO = 0x1,
    SP_DRIFT_MODE_DEFAULT = SP_DRIFT_MODE_MANUAL,
};

enum {
    SP_SETTING_HUD_LABELS_HIDE = 0x0,
    SP_SETTING_HUD_LABELS_SHOW = 0x1,
    SP_SETTING_HUD_LABELS_DEFAULT = SP_SETTING_HUD_LABELS_SHOW,
};

enum {
    SP_SETTING_169_FOV_169 = 0x0,
    SP_SETTING_169_FOV_43 = 0x1,
    SP_SETTING_169_FOV_DEFAULT = SP_SETTING_169_FOV_169,
};

enum {
    SP_SETTING_MAP_ICONS_CHARACTERS = 0x0,
    SP_SETTING_MAP_ICONS_MIIS = 0x1,
    SP_SETTING_MAP_ICONS_DEFAULT = SP_SETTING_MAP_ICONS_MIIS,
};

enum {
    SP_TA_RULE_CLASS_150CC = 0x0,
    SP_TA_RULE_CLASS_200CC = 0x1,
    SP_TA_RULE_CLASS_DEFAULT = SP_TA_RULE_CLASS_150CC,
};

enum {
    SP_TA_RULE_GHOST_SORTING_FASTEST = 0x0,
    SP_TA_RULE_GHOST_SORTING_SLOWEST = 0x1,
    SP_TA_RULE_GHOST_SORTING_NEWEST = 0x2,
    SP_TA_RULE_GHOST_SORTING_OLDEST = 0x3,
    SP_TA_RULE_GHOST_SORTING_DEFAULT = SP_TA_RULE_GHOST_SORTING_FASTEST,
};

enum {
    SP_TA_RULE_GHOST_TAG_VISIBILITY_NONE = 0x0,
    SP_TA_RULE_GHOST_TAG_VISIBILITY_WATCHED = 0x1,
    SP_TA_RULE_GHOST_TAG_VISIBILITY_ALL = 0x2,
    SP_TA_RULE_GHOST_TAG_VISIBILITY_DEFAULT = SP_TA_RULE_GHOST_TAG_VISIBILITY_ALL,
};

enum {
    SP_TA_RULE_GHOST_TAG_CONTENT_NAME = 0x0,
    SP_TA_RULE_GHOST_TAG_CONTENT_TIME = 0x1,
    SP_TA_RULE_GHOST_TAG_CONTENT_TIME_NOLEADING = 0x2,
    SP_TA_RULE_GHOST_TAG_CONTENT_DATE = 0x3,
    SP_TA_RULE_GHOST_TAG_CONTENT_DEFAULT = SP_TA_RULE_GHOST_TAG_CONTENT_NAME,
};

enum {
    SP_TA_RULE_SOLID_GHOSTS_NONE = 0x0,
    SP_TA_RULE_SOLID_GHOSTS_WATCHED = 0x1,
    SP_TA_RULE_SOLID_GHOSTS_ALL = 0x2,
    SP_TA_RULE_SOLID_GHOSTS_DEFAULT = SP_TA_RULE_SOLID_GHOSTS_NONE,
};

enum {
    SP_SETTING_PAGE_TRANSITIONS_DISABLE = 0x0,
    SP_SETTING_PAGE_TRANSITIONS_ENABLE = 0x1,
    SP_SETTING_PAGE_TRANSITIONS_DEFAULT = SP_SETTING_PAGE_TRANSITIONS_ENABLE,
};

enum {
    SP_SETTING_RACE_INPUT_DISPLAY_DISABLE = 0x0,
    SP_SETTING_RACE_INPUT_DISPLAY_SIMPLE = 0x1,
    SP_SETTING_RACE_INPUT_DISPLAY_CLASSIC = 0x2,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET2 = 0x3,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET3 = 0x4,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET4 = 0x5,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET5 = 0x6,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET6 = 0x7,
    SP_SETTING_RACE_INPUT_DISPLAY_DEFAULT = SP_SETTING_RACE_INPUT_DISPLAY_SIMPLE,
};

enum {
    SP_TA_RULE_GHOST_SOUND_NONE = 0x0,
    SP_TA_RULE_GHOST_SOUND_WATCHED = 0x1,
    SP_TA_RULE_GHOST_SOUND_ALL = 0x2,
    SP_TA_RULE_GHOST_SOUND_DEFAULT = SP_TA_RULE_GHOST_SOUND_WATCHED,
};

enum {
    kDriftMode_Manual,
    kDriftMode_Auto,

    kDriftMode_MAX,
    kDriftMode_Default = kDriftMode_Manual,
};
enum {
    kHudLabels_Hide,
    kHudLabels_Show,
    kHudLabels_MAX,

    kHudLabels_Default = kHudLabels_Show,
};

enum {
    kFOV_169,
    kFOV_43,
    kFOV_MAX,
    kFOV_Default = kFOV_169,
};

enum {
    kMapIcons_Characters,
    kMapIcons_Miis,

    kMapIcons_MAX,
    kMapIcons_Default = kMapIcons_Miis,
};

enum {
    kTaRuleClass_150cc,
    kTaRuleClass_200cc,

    kTaRuleClass_MAX,
    kTaRuleClass_Default = kTaRuleClass_150cc,
};

enum {
    kTaRuleGhostSorting_Fastest,
    kTaRuleGhostSorting_Slowest,
    kTaRuleGhostSorting_Newest,
    kTaRuleGhostSorting_Oldest,

    kTaRuleGhostSorting_MAX,
    kTaRuleGhostSorting_Default = kTaRuleGhostSorting_Fastest,
};

enum {
    kTaRuleGhostTagVisibility_None,
    kTaRuleGhostTagVisibility_Watched,
    kTaRuleGhostTagVisibility_All,

    kTaRuleGhostTagVisibility_MAX,
    kTaRuleGhostTagVisibility_Default = kTaRuleGhostTagVisibility_All,
};

enum {
    kTaRuleGhostTagContent_Name,
    kTaRuleGhostTagContent_Time,
    kTaRuleGhostTagContent_TimeNoLeading,
    kTaRuleGhostTagContent_Date,

    kTaRuleGhostTagContent_MAX,
    kTaRuleGhostTagContent_Default = kTaRuleGhostTagContent_Name,
};

enum {
    kTaRuleSolidGhosts_None,
    kTaRuleSolidGhosts_Watched,
    kTaRuleSolidGhosts_All,

    kTaRuleSolidGhosts_MAX,
    kTaRuleSolidGhosts_Default = kTaRuleSolidGhosts_None,
};
enum {
    kPageTransitions_Disable,
    kPageTransitions_Enable,

    kPageTransitions_MAX,
    kPageTransitions_Default = kPageTransitions_Enable,
};

enum {
    kRaceInputDisplay_Disable,
    kRaceInputDisplay_Simple,

    kRaceInputDisplay_MAX,
    kRaceInputDisplay_Default = kRaceInputDisplay_Simple,
};

enum {
    kTaRuleGhostSound_None,
    kTaRuleGhostSound_Watched,
    kTaRuleGhostSound_All,

    kTaRuleGhostSound_MAX,
    kTaRuleGhostSound_Default = kTaRuleGhostSound_Watched,
};

// NOTE: Mirrored by ASM in Common.S, modify both when making changes
typedef enum {
    kSetting_DriftMode = 0,
    kSetting_HudLabels = 1,
    kSetting_169_Fov = 2,
    kSetting_MapIcons = 3,
    kSetting_TaRuleClass = 4,
    kSetting_TaRuleGhostSorting = 5,
    kSetting_TaRuleGhostTagVisibility = 6,
    kSetting_TaRuleGhostTagContent = 7,
    kSetting_TaRuleSolidGhosts = 8,
    kSetting_PageTransitions = 9,
    kSetting_RaceInputDisplay = 10,
    kSetting_TaRuleGhostSound = 11,

    // BE encoding of u8[4]
    kSetting_MiiAvatar = 12,
    // BE encoding of u8[4]
    kSetting_MiiClient = 13,

    kSetting_MAX,
} SpSettingKey;

enum {
    kCategory_Race,
    kCategory_TA,
    kCategory_License,

    kCategory_MAX,
};

const BaseSettingsDescriptor *ClientSettings_getDescriptor(void);

typedef struct {
    const BaseSettingsDescriptor *mDesc;
    u32 mValues[kSetting_MAX];
} ClientSettings;

static inline void ClientSettings_init(ClientSettings *self) {
    self->mDesc = ClientSettings_getDescriptor();
    assert(self->mDesc->numValues == kSetting_MAX);
}
static inline void ClientSettings_reset(ClientSettings *self) {
    SpSetting_ResetToDefault(self->mDesc, self->mValues);
}
static inline void ClientSettings_readIni(ClientSettings *self, StringView view) {
    SpSetting_ParseFromIni(view.s, view.len, self->mDesc, self->mValues);
}
static inline void ClientSettings_writeIni(
        const ClientSettings *self, char *start, size_t len) {
    const char *header = "# MKW-SP Settings\n";
    size_t written = snprintf(start, len, "%s", header);
    start += written;
    len -= written;
    SpSetting_WriteToIni(start, len, self->mDesc, self->mValues);
}
static inline void ClientSettings_set(
        ClientSettings *self, const char *key, const char *value) {
    SpSetting_Set(self->mDesc, self->mValues, key, value);
}
