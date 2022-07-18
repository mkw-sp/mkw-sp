#pragma once

enum {
    kDriftMode_Manual,
    kDriftMode_Auto,

    kDriftMode_MAX,
    kDriftMode_Default = kDriftMode_Manual,
};

enum {
    kVanillaMode_Disable,
    kVanillaMode_Enable,
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
