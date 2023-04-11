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
    kTaRuleSolidGhosts_None,
    kTaRuleSolidGhosts_Watched,
    kTaRuleSolidGhosts_All,

    kTaRuleSolidGhosts_MAX,
    kTaRuleSolidGhosts_Default = kTaRuleSolidGhosts_None,
};

enum {
    kTaRuleGhostSound_None,
    kTaRuleGhostSound_Watched,
    kTaRuleGhostSound_All,

    kTaRuleGhostSound_MAX,
    kTaRuleGhostSound_Default = kTaRuleGhostSound_Watched,
};
