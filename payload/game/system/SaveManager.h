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
    SP_SAVE_LICENSE_VERSION = 0,
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
    SP_DRIFT_MODE_AUTO = 0x0,
    SP_DRIFT_MODE_MANUAL = 0x1,
};

enum {
    SP_SETTING_HUD_LABELS_HIDE = 0x0,
    SP_SETTING_HUD_LABELS_SHOW = 0x1,
};

enum {
    SP_SETTING_169_FOV_DEFAULT = 0x0,
    SP_SETTING_169_FOV_WIDER = 0x1,
};

enum {
    SP_TA_RULE_CLASS_150CC = 0x0,
    SP_TA_RULE_CLASS_200CC = 0x1,
};

enum {
    SP_TA_RULE_GHOST_TAGS_NONE = 0x0,
    SP_TA_RULE_GHOST_TAGS_WATCHED = 0x1,
    SP_TA_RULE_GHOST_TAGS_ALL = 0x2,
};

enum {
    SP_TA_RULE_SOLID_GHOSTS_NONE = 0x0,
    SP_TA_RULE_SOLID_GHOSTS_WATCHED = 0x1,
    SP_TA_RULE_SOLID_GHOSTS_ALL = 0x2,
};

typedef struct {
    SpSaveSection;
    MiiId miiId;
    u32 driftMode : 1;
    u32 settingHudLabels : 1;
    u32 setting169Fov : 1;
    u32 taRuleClass : 1;
    u32 taRuleGhostTags : 2;
    u32 taRuleSolidGhosts : 2;
} SpSaveLicense;

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
    char (*ghostPaths)[NAND_MAX_PATH]; // Modified
    u8 _00034[0x00038 - 0x00034];
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
    bool *courseSha1IsValid; // Added
    u32 (*courseSha1s)[5]; // Added
} SaveManager;
static_assert(offsetof(SaveManager, spBuffer) == 0x25008);

extern SaveManager *s_saveManager;

SaveManager *SaveManager_createInstance(void);

void SaveManager_initAsync(SaveManager *this);

void SaveManager_resetAsync(SaveManager *this);

void SaveManager_saveLicensesAsync(SaveManager *this);

void SaveManager_createLicense(SaveManager *this, u32 licenseId, const MiiId *miiId, const wchar_t *miiName);

void SaveManager_selectLicense(SaveManager *this, u32 licenseId);

void SaveManager_eraseSpLicense(SaveManager *this);

void SaveManager_createSpLicense(SaveManager *this, const MiiId *miiId);

u32 SaveManager_getSettingHudLabels(const SaveManager *this);

u32 SaveManager_getSetting169Fov(const SaveManager *this);

u32 SaveManager_getTaRuleClass(const SaveManager *this);

u32 SaveManager_getTaRuleGhostTags(const SaveManager *this);

u32 SaveManager_getTaRuleSolidGhosts(const SaveManager *this);

void SaveManager_loadGhostAsync(SaveManager *this, s32 licenseId, u32 category, u32 index, u32 courseId);

void SaveManager_loadGhostHeadersAsync(SaveManager *this, s32 licenseId, GhostGroup *group);

bool SaveManager_computeCourseSha1Async(SaveManager *this, u32 courseId);

const u32 *SaveManager_getCourseSha1(const SaveManager *this, u32 courseId);

extern bool vsSpeedModIsEnabled;
