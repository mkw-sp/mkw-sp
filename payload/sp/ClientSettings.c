#include "ClientSettings.h"

static const char *kDriftMode[kDriftMode_MAX] = {
    [kDriftMode_Manual] = "Manual",
    [kDriftMode_Auto] = "Auto",
};
static const char *kHudLabels[] = {
    [kHudLabels_Hide] = "Hide",
    [kHudLabels_Show] = "Show",
};
static const char *kFOV[] = {
    [kFOV_169] = "16:9",
    [kFOV_43] = "4:3",
};
static const char *kMapIcons[] = {
    [kMapIcons_Characters] = "Characters",
    [kMapIcons_Miis] = "Miis",
};
static const char *kTaRuleClass[] = {
    [kTaRuleClass_150cc] = "150cc",
    [kTaRuleClass_200cc] = "200cc",
};
static const char *kTaRuleGhostSorting[] = {
    [kTaRuleGhostSorting_Fastest] = "Fastest",
    [kTaRuleGhostSorting_Slowest] = "Slowest",
    [kTaRuleGhostSorting_Newest] = "Newest",
    [kTaRuleGhostSorting_Oldest] = "Oldest",
};
static const char *kTaRuleGhostTagVisibility[] = {
    [kTaRuleGhostTagVisibility_None] = "None",
    [kTaRuleGhostTagVisibility_Watched] = "Watched",
    [kTaRuleGhostTagVisibility_All] = "All",
};
static const char *kTaRuleGhostTagContent[] = {
    [kTaRuleGhostTagContent_Name] = "Name",
    [kTaRuleGhostTagContent_Time] = "Time",
    [kTaRuleGhostTagContent_TimeNoLeading] = "TimeNoLeading",
    [kTaRuleGhostTagContent_Date] = "Date",
};
static const char *kTaRuleSolidGhosts[] = {
    [kTaRuleSolidGhosts_None] = "None",
    [kTaRuleSolidGhosts_Watched] = "Watched",
    [kTaRuleSolidGhosts_All] = "All",
};
static const char *kPageTransitions[] = {
    [kPageTransitions_Disable] = "Disable",
    [kPageTransitions_Enable] = "Enable",
};
static const char *kRaceInputDisplay[] = {
    [kRaceInputDisplay_Disable] = "Disable",
    [kRaceInputDisplay_Simple] = "Simple",
};
static const char *kTaRuleGhostSound[] = {
    [kTaRuleGhostSound_None] = "None",
    [kTaRuleGhostSound_Watched] = "Watched",
    [kTaRuleGhostSound_All] = "All",
};

static const char *kCategory[] = {
    [kCategory_Race] = "Race",
    [kCategory_TA] = "TTs",
};

#define REGISTER_SETTING(Id, Name, Category, Enum)                          \
    [Id] = (Setting) {                                                      \
        .name = Name, .category = Category, .defaultValue = Enum##_Default, \
        .enumValues = Enum, .numEnumValues = Enum##_MAX                     \
    }

static const Setting SpSettings[] = {
    // clang-format off
    REGISTER_SETTING(kSetting_DriftMode, "DriftMode", kCategory_Race, kDriftMode),
    REGISTER_SETTING(kSetting_HudLabels, "HudLabels", kCategory_Race, kHudLabels),
    REGISTER_SETTING(kSetting_169_Fov, "FOV", kCategory_Race, kFOV),
    REGISTER_SETTING(kSetting_MapIcons, "MapIcons", kCategory_Race, kMapIcons),
    REGISTER_SETTING(kSetting_TaRuleClass, "Class", kCategory_TA, kTaRuleClass),
    REGISTER_SETTING(kSetting_TaRuleGhostSorting, "GhostSorting", kCategory_TA, kTaRuleGhostSorting),
    REGISTER_SETTING(kSetting_TaRuleGhostTagVisibility, "TagVisibility", kCategory_TA, kTaRuleGhostTagVisibility),
    REGISTER_SETTING(kSetting_TaRuleGhostTagContent, "TagContent", kCategory_TA, kTaRuleGhostTagContent),
    REGISTER_SETTING(kSetting_TaRuleSolidGhosts, "SolidGhosts", kCategory_TA, kTaRuleSolidGhosts),
    REGISTER_SETTING(kSetting_PageTransitions, "PageTransitions", kCategory_Race, kPageTransitions),
    REGISTER_SETTING(kSetting_RaceInputDisplay, "InputDisplay", kCategory_Race, kRaceInputDisplay),
    REGISTER_SETTING(kSetting_TaRuleGhostSound, "GhostSound", kCategory_TA, kTaRuleGhostSound),
    // clang-format on
};
static const BaseSettingsDescriptor SpSettingsDesc = (BaseSettingsDescriptor){
    .numValues = ARRAY_SIZE(SpSettings),
    .fieldDescriptors = SpSettings,
    .categoryNames = kCategory,
};

const BaseSettingsDescriptor *ClientSettings_getDescriptor(void) {
    return &SpSettingsDesc;
}
