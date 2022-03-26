#pragma once

#include "GhostFile.h"

#include <egg/core/eggTaskThread.h>

#include <revolution.h>

#include <game/system/SpSetting.h>

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

typedef struct {
    SpSaveSection;
    MiiId miiId;
    SpSettingBitField;
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
    NodeId *ghostIds; // Modified
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
