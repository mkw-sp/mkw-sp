#pragma once

#include "GhostFile.h"

#include <egg/core/eggTaskThread.h>

#include <revolution.h>

enum {
    VS_RULE_CLASS_100CC = 0x0,
    VS_RULE_CLASS_150CC = 0x1,
    VS_RULE_CLASS_MIRROR = 0x2,
    VS_RULE_CLASS_200CC = 0x3,
};

enum {
    SP_SAVE_HEADER_MAGIC = 0x53505341, // SPSA
    SP_SAVE_LICENSE_MAGIC = 0x53504c49, // SPLI
};

enum {
    SP_SAVE_LICENSE_VERSION = 3,
};

typedef struct {
    u32 magic;
    u32 crc32;
} SpSaveHeader;

typedef struct {
    u32 magic;
    u32 size;
    u32 version;
} SpSaveSection;

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
    SP_SETTING_PAGE_TRANSITIONS_DISABLE = 0x0,
    SP_SETTING_PAGE_TRANSITIONS_ENABLE = 0x1,
    SP_SETTING_PAGE_TRANSITIONS_DEFAULT = SP_SETTING_PAGE_TRANSITIONS_ENABLE,
};

enum {
    SP_SETTING_RACE_INPUT_DISPLAY_DISABLE = 0x0,
    SP_SETTING_RACE_INPUT_DISPLAY_SIMPLE = 0x1,   // Preset 0
    SP_SETTING_RACE_INPUT_DISPLAY_CLASSIC = 0x2,  // Preset 1
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET2 = 0x3,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET3 = 0x4,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET4 = 0x5,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET5 = 0x6,
    SP_SETTING_RACE_INPUT_DISPLAY_PRESET6 = 0x7,
    SP_SETTING_RACE_INPUT_DISPLAY_DEFAULT = SP_SETTING_RACE_INPUT_DISPLAY_SIMPLE,
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
    SP_TA_RULE_GHOST_SOUND_NONE = 0x0,
    SP_TA_RULE_GHOST_SOUND_WATCHED = 0x1,
    SP_TA_RULE_GHOST_SOUND_ALL = 0x2,
    SP_TA_RULE_GHOST_SOUND_DEFAULT = SP_TA_RULE_GHOST_SOUND_WATCHED,
};

typedef struct {
    SpSaveSection;
    MiiId miiId;
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
} SpSaveLicense;
static_assert(sizeof(SpSaveLicense) == 0x18);

enum {
    SP_BUFFER_SIZE = 0x10000, // 64 KiB
};

enum {
    MAX_SP_LICENSE_COUNT = 6,
};

typedef struct {
    wchar_t miiName[11];
    MiiId miiId;
    u8 _001e[0x93f0 - 0x001e];
} License;
static_assert(sizeof(License) == 0x93f0);

typedef struct {
    u8 _00000[0x00014 - 0x00000];
    void *rawSave;
    u8 *rawGhostFile;
    EGG_TaskThread *taskThread;
    u32 ghostCount; // Modified
    RawGhostHeader *rawGhostHeaders; // Modified
    GhostGroup *ghostGroup;
    GhostFooter *ghostFooters; // Modified
    const wchar_t **ghostPaths;
    u8 _00034[0x00035 - 0x00034];
    bool saveGhostResult;
    u8 _00036[0x00038 - 0x00036];
    License licenses[4];
    u8 _24ff8[0x24ffc - 0x24ff8];
    void *otherRawSave;
    bool isBusy;
    bool isValid;
    bool canSave;
    bool spCanSave; // Added (was padding)
    u32 result;
    void *spBuffer; // Added
    u32 spSectionCount; // Added
    SpSaveSection **spSections; // Added
    u32 spLicenseCount; // Added
    SpSaveLicense *spLicenses[MAX_SP_LICENSE_COUNT]; // Added
    s32 spCurrentLicense; // Added
    wchar_t *ghostPathBuffer; // Added
    u32 ghostPathBufferFreeCount;
    bool *courseSha1IsValid; // Added
    u8 (*courseSha1s)[0x14]; // Added
} SaveManager;
static_assert(offsetof(SaveManager, spBuffer) == 0x25008);

extern SaveManager *s_saveManager;

SaveManager *SaveManager_createInstance(void);

void SaveManager_initAsync(SaveManager *this);

void SaveManager_resetAsync(SaveManager *this);

void SaveManager_saveLicensesAsync(SaveManager *this);

void SaveManager_createLicense(SaveManager *this, u32 licenseId, const MiiId *miiId,
        const wchar_t *miiName);

void SaveManager_selectLicense(SaveManager *this, u32 licenseId);

void SaveManager_eraseSpLicense(SaveManager *this);

void SaveManager_createSpLicense(SaveManager *this, const MiiId *miiId);

u32 SaveManager_getDriftMode(const SaveManager *this);

void SaveManager_setDriftMode(SaveManager *this, u32 driftMode);

u32 SaveManager_getSettingHudLabels(const SaveManager *this);

u32 SaveManager_getSetting169Fov(const SaveManager *this);

u32 SaveManager_getSettingMapIcons(const SaveManager *this);

u32 SaveManager_getSettingPageTransitions(const SaveManager *this);

void SaveManager_setSettingPageTransitions(const SaveManager *this, u32 pageTransitions);

u32 SaveManager_getSettingRaceInputDisplay(const SaveManager *this);

void SaveManager_setSettingRaceInputDisplay(SaveManager *this, u32 raceInputDisplay);

u32 SaveManager_getTaRuleClass(const SaveManager *this);

u32 SaveManager_getTaRuleGhostSorting(const SaveManager *this);

u32 SaveManager_getTaRuleGhostTagVisibility(const SaveManager *this);

u32 SaveManager_getTaRuleGhostTagContent(const SaveManager *this);

u32 SaveManager_getTaRuleSolidGhosts(const SaveManager *this);

u32 SaveManager_getTaRuleGhostSound(const SaveManager *this);

void SaveManager_loadGhostAsync(SaveManager *this, s32 licenseId, u32 category, u32 index,
        u32 courseId);

void SaveManager_loadGhostHeadersAsync(SaveManager *this, s32 licenseId, GhostGroup *group);

void SaveManager_saveGhostAsync(SaveManager *this, s32 licenseId, u32 category, u32 index,
        GhostFile *file, bool saveLicense);

bool SaveManager_computeCourseSha1Async(SaveManager *this, u32 courseId);

const u8 *SaveManager_getCourseSha1(const SaveManager *this, u32 courseId);

extern bool vsSpeedModIsEnabled;
