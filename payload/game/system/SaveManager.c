#include "SaveManager.h"

#include "NandHelper.h"
#include "RaceConfig.h"
#include "ResourceManager.h"
#include "RootScene.h"
#include <sp/Yaz.h>

#include "../ui/SectionManager.h"

#include <stdalign.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

enum {
    GHOST_INIT_STACK_SIZE = 0x8000, // 32 KiB
};

SaveManager *SaveManager_ct(SaveManager *this);

SaveManager *my_SaveManager_createInstance(void) {
    s_saveManager = new(sizeof(SaveManager));
    SaveManager_ct(s_saveManager);
    SaveManager *this = s_saveManager;

    this->ghostCount = 0;
    EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM1];
    this->rawGhostHeaders = spAllocArray(MAX_GHOST_COUNT, sizeof(RawGhostHeader), 0x4, heap);
    this->ghostFooters = spAllocArray(MAX_GHOST_COUNT, sizeof(GhostFooter), 0x4, heap);
    this->ghostIds = spAllocArray(MAX_GHOST_COUNT, sizeof(NodeId), 0x4, heap);
    this->ghostInitStack = spAlloc(GHOST_INIT_STACK_SIZE, 0x4, heap);
    this->courseSha1IsValid = spAllocArray(0x20, sizeof(bool), 0x4, heap);
    for (u32 i = 0; i < 0x20; i++) {
        this->courseSha1IsValid[i] = false;
    }
    this->courseSha1s = spAllocArray(0x20, 0x14, 0x4, heap);

    this->spCanSave = true;
    this->spLicenseCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->spLicenses); i++) {
        this->spLicenses[i] = spAlloc(sizeof(SpSaveLicense), 0x4, heap);
        ClientSettings_init(&this->spLicenses[i]->cfg);
        ClientSettings_reset(&this->spLicenses[i]->cfg);
    }
    this->spCurrentLicense = -1;

    return s_saveManager;
}
PATCH_B(SaveManager_createInstance, my_SaveManager_createInstance);

static void SaveManager_initGhost(SaveManager *this, const NodeId id) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    u32 readSize;
    if (!Storage_fastReadFile(id, this->rawGhostFile, 0x2800, &readSize)) {
        return;
    }

    if (!RawGhostFile_spIsValid(this->rawGhostFile, readSize)) {
        return;
    }

    const RawGhostHeader *header = (RawGhostHeader *)this->rawGhostFile;
    memcpy(&this->rawGhostHeaders[this->ghostCount], header, sizeof(RawGhostHeader));
    GhostFooter_init(&this->ghostFooters[this->ghostCount], this->rawGhostFile, readSize);
    this->ghostIds[this->ghostCount] = id;
    this->ghostCount++;
}

static void SaveManager_initGhostsDir(SaveManager *this, wchar_t *path, u32 offset) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    Dir dir;
    if (!Storage_openDir(&dir, path)) {
        return;
    }

    NodeInfo info;
    while (Storage_readDir(&dir, &info)) {
        if (info.type == NODE_TYPE_NONE) {
            break;
        }
        s32 length = swprintf(path + offset, 2048 - offset, L"/%ls", info.name);
        if (length < 0) {
            continue;
        }
        offset += length;
        if (info.type == NODE_TYPE_FILE) {
            SaveManager_initGhost(this, info.id);
        } else if (info.type == NODE_TYPE_DIR) {
            SaveManager_initGhostsDir(this, path, offset);
        } else {
            break;
        }
        offset -= length;
    }

    Storage_closeDir(&dir);
}

static void SaveManager_initGhosts(SaveManager *this) {
    SP_LOG("Initializing ghosts...");

    wchar_t path[2048];
    u32 offset = swprintf(path, 2048, L"/mkw-sp/ghosts");
    SaveManager_initGhostsDir(this, path, offset);
    offset = swprintf(path, 2048, L"/ctgpr/ghosts");
    SaveManager_initGhostsDir(this, path, offset);

    Storage_createDir(L"/mkw-sp/ghosts", true);

    SP_LOG("Ghosts: %u / %u", this->ghostCount, MAX_GHOST_COUNT);
}

static void *SaveManager_initGhostsTask(void *UNUSED(arg)) {
    SaveManager_initGhosts(s_saveManager);
    return NULL;
}

static void SaveManager_initGhostsAsync(SaveManager *this) {
    void *stackBase = this->ghostInitStack + GHOST_INIT_STACK_SIZE;
    OSCreateThread(&this->ghostInitThread, SaveManager_initGhostsTask, NULL, stackBase,
            GHOST_INIT_STACK_SIZE, 31, 0);
    OSResumeThread(&this->ghostInitThread);
}

static bool SaveManager_initSpSave(SaveManager *this) {
    this->spLicenseCount = 0;
    
    // TODO: Hopefully this is enough. Can always stream the file if not.
    char iniBuffer[512];

    for (size_t i = 0; i < 6; ++i) {
        wchar_t path[64];
        swprintf(path, ARRAY_SIZE(path), L"/mkw-sp/settings%u.ini", (unsigned)i);

        u32 size;
        if (!Storage_readFile(path, iniBuffer, sizeof(iniBuffer), &size)) {
            break;
        }

        ClientSettings_readIni(&this->spLicenses[i]->cfg, (StringView){ .s = iniBuffer, .len = size });
        this->spLicenseCount = i + 1;
    }

    return true;
}

static void SaveManager_init(SaveManager *this) {
    this->isValid = true;
    this->canSave = false;

    this->otherRawSave = this->rawSave;

    if (!SaveManager_initSpSave(this)) {
        EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM1];
        for (u32 i = 0; i < MAX_SP_LICENSE_COUNT; i++) {
            this->spLicenses[i] = spAlloc(sizeof(SpSaveLicense), 0x4, heap);
        }
        this->result = RK_NAND_RESULT_NOSPACE;
        this->spCanSave = false;
    }

    SaveManager_initGhostsAsync(this);

    this->isBusy = false;
}

static void initTask(void *UNUSED(arg)) {
    SaveManager_init(s_saveManager);
}

static void my_SaveManager_initAsync(SaveManager *this) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, initTask, NULL, NULL);
}
PATCH_B(SaveManager_initAsync, my_SaveManager_initAsync);

static void my_SaveManager_resetAsync(SaveManager *this) {
    this->isValid = true;
    this->canSave = false;

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}
PATCH_B(SaveManager_resetAsync, my_SaveManager_resetAsync);

void RawLicense_reset(RawLicense *self);

static void my_RawLicense_reset(RawLicense *self) {
    memset(self, 0, sizeof(*self));
    self->unlockFlags[0] = 0xffffffff;
    self->unlockFlags[1] = 0x31ffffc;
    self->rules[0] = 0x5008;
    self->rules[1] = 0x1000;
    self->rules[2] = 0x5008;
    self->rules[3] = 0x1000;
    self->vr = 5000;
    self->br = 5000;
    self->driftMode = (SaveManager_getSetting(s_saveManager, kSetting_DriftMode) + 1) << 14;
}
PATCH_B(RawLicense_reset, my_RawLicense_reset);

static bool TryReplace(const wchar_t* path, const void* fileData, size_t fileSize) {
    wchar_t newPath[64];
    swprintf(newPath, ARRAY_SIZE(newPath), L"%s.new", path);
    wchar_t oldPath[64];
    swprintf(oldPath, ARRAY_SIZE(oldPath), L"%s.old", path);

    if (!Storage_writeFile(newPath, true, fileData, fileSize)) {
        SP_LOG("Failed to write to %ls", newPath);
        return false;
    }

    if (!Storage_remove(oldPath, true)) {
        SP_LOG("Failed to remove %ls", oldPath);
        return false;
    }

    if (!Storage_rename(path, oldPath)) {
        SP_LOG("Failed to rename %ls to %ls", path, oldPath);
        // Ignore
    }

    if (!Storage_rename(newPath, path)) {
        SP_LOG("Failed to rename %ls to %ls", newPath, path);
        return false;
    }

    Storage_remove(oldPath, true); // Not a big deal if this fails
    return true;
}

static void SaveManager_saveSp(SaveManager *this) {
    char iniBuffer[512];

    for (size_t i = 0; i < this->spLicenseCount; ++i) {
        SpSaveLicense* license = this->spLicenses[i];

        ClientSettings_writeIni(&license->cfg, iniBuffer, sizeof(iniBuffer));
        SP_LOG("Writing settings: %s", iniBuffer);

        wchar_t path[64];
        swprintf(path, ARRAY_SIZE(path), L"/mkw-sp/settings%u.ini", (unsigned)i);

        if (!TryReplace(path, iniBuffer, strlen(iniBuffer))) {
            SP_LOG("Failed to save %ls", path);
            this->spCanSave = false;
            this->isBusy = false;
            this->result = RK_NAND_RESULT_NOSPACE;
            return;
        }
    }

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}

static void saveSpTask(void *UNUSED(arg)) {
    SaveManager_saveSp(s_saveManager);
}

static void my_SaveManager_saveLicensesAsync(SaveManager *this) {
    if (!this->spCanSave) {
        this->isBusy = false;
        this->result = RK_NAND_RESULT_OK;
    }

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, saveSpTask, NULL, NULL);
}
PATCH_B(SaveManager_saveLicensesAsync, my_SaveManager_saveLicensesAsync);

void SaveManager_eraseSpLicense(SaveManager *this) {
    for (u32 i = this->spCurrentLicense; i < this->spLicenseCount - 1; i++) {
        this->spLicenses[i] = this->spLicenses[i + 1];
    }
    this->spLicenseCount--;
    this->spCurrentLicense = -1;
}

static void ClientSettings_setMii(ClientSettings* self, const MiiId* miiId) {
    const u32 miiAvatar = (miiId->avatar[0] << 24) | (miiId->avatar[1] << 16) | (miiId->avatar[2] << 8) | miiId->avatar[3]; 
    const u32 miiClient = (miiId->client[0] << 24) | (miiId->client[1] << 16) | (miiId->client[2] << 8) | miiId->client[3];
    self->mValues[kSetting_MiiAvatar] = miiAvatar;
    self->mValues[kSetting_MiiClient] = miiClient;
}
static MiiId ClientSettings_getMii(const ClientSettings *self) {
    assert(self != NULL);

    const u32 miiAvatar = self->mValues[kSetting_MiiAvatar];
    const u32 miiClient = self->mValues[kSetting_MiiClient];

    return (MiiId) {
        .avatar = {
            miiAvatar >> 24, miiAvatar >> 16, miiAvatar >> 8, miiAvatar
        },
        .client = {
            miiClient >> 24, miiClient >> 16, miiClient >> 8, miiClient
        }
    };
}
MiiId SaveManager_getMiiId(const SaveManager *this, u32 licenseId) {
    assert(licenseId < this->spLicenseCount);

    const SpSaveLicense *license = this->spLicenses[licenseId];
    assert(license != NULL);

    return ClientSettings_getMii(&license->cfg);
}

void SaveManager_createSpLicense(SaveManager *this, const MiiId *miiId) {
    SpSaveLicense *license = this->spLicenses[this->spLicenseCount++];
    
    ClientSettings_init(&license->cfg);
    ClientSettings_reset(&license->cfg);
    ClientSettings_setMii(&license->cfg, miiId);

    this->spCurrentLicense = this->spLicenseCount - 1;
}

void SaveManager_changeSpLicenseMiiId(const SaveManager *this, const MiiId *miiId) {
    if (this->spCurrentLicense < 0) {
        return;
    }

    ClientSettings_setMii(&this->spLicenses[this->spCurrentLicense]->cfg, miiId);
} 

u32 SaveManager_getSetting(const SaveManager *this, SpSettingKey key) {
    assert(key < kSetting_MAX);

    if (this->spCurrentLicense < 0) {
        const BaseSettingsDescriptor* desc = ClientSettings_getDescriptor();
        assert(key < desc->numValues);

        const Setting* field = &desc->fieldDescriptors[key];
        return field->defaultValue;
    }

    assert(this->spLicenses[this->spCurrentLicense]->cfg.mDesc);
    return this->spLicenses[this->spCurrentLicense]->cfg.mValues[key];
}

void SaveManager_setSetting(SaveManager *this, SpSettingKey key, u32 value) {
    assert(key < kSetting_MAX);

    if (this->spCurrentLicense < 0) {
        return;
    }

    this->spLicenses[this->spCurrentLicense]->cfg.mValues[key] = value;
}

static void SaveManager_loadGhostHeaders(SaveManager *this) {
    OSJoinThread(&this->ghostInitThread, NULL);
    OSDetachThread(&this->ghostInitThread);

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}

static void loadGhostHeadersTask(void *UNUSED(arg)) {
    SaveManager_loadGhostHeaders(s_saveManager);
}

static void my_SaveManager_loadGhostHeadersAsync(SaveManager *this, s32 UNUSED(licenseId),
        GhostGroup *UNUSED(group)) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, loadGhostHeadersTask, NULL, NULL);
}
PATCH_B(SaveManager_loadGhostHeadersAsync, my_SaveManager_loadGhostHeadersAsync);

static bool SaveManager_loadGhost(SaveManager *this, u32 i) {
    const GlobalContext *cx = s_sectionManager->globalContext;
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;

    NodeId id = this->ghostIds[cx->timeAttackGhostIndices[i]];
    u32 readSize;
    if (!Storage_fastReadFile(id, this->rawGhostFile, 0x2800, &readSize)) {
        return false;
    }

    if (((RawGhostHeader *)this->rawGhostFile)->isCompressed) {
        if (!RawGhostFile_spIsValid(this->rawGhostFile, readSize)) {
            return false;
        }

        if (!RawGhostFile_spDecompress(this->rawGhostFile, (*menuScenario->ghostBuffer)[i])) {
            return false;
        }
    } else {
        memcpy((*menuScenario->ghostBuffer)[i], this->rawGhostFile, 0x2800);
    }

    return RawGhostFile_spIsValid((*menuScenario->ghostBuffer)[i], 0x2800);
}

static void SaveManager_loadGhosts(SaveManager *this) {
    OSJoinThread(&this->ghostInitThread, NULL);
    OSDetachThread(&this->ghostInitThread);

    RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    if (menuScenario->ghostBuffer == raceScenario->ghostBuffer) {
        if (menuScenario->ghostBuffer == s_raceConfig->ghostBuffers + 0) {
            menuScenario->ghostBuffer = s_raceConfig->ghostBuffers + 1;
        } else {
            menuScenario->ghostBuffer = s_raceConfig->ghostBuffers + 0;
        }
    }

    const GlobalContext *cx = s_sectionManager->globalContext;
    for (u32 i = 0; i < cx->timeAttackGhostCount; i++) {
        if (!SaveManager_loadGhost(this, i)) {
            memset((*menuScenario->ghostBuffer)[i], 0, 0x2800);
        }
    }

    this->isBusy = false;
}

static void loadGhostsTask(void *UNUSED(arg)) {
    SaveManager_loadGhosts(s_saveManager);
}

static void my_SaveManager_loadGhostAsync(SaveManager *this, s32 UNUSED(licenseId),
        u32 UNUSED(category), u32 UNUSED(index), u32 UNUSED(courseId)) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, loadGhostsTask, NULL, NULL);
}
PATCH_B(SaveManager_loadGhostAsync, my_SaveManager_loadGhostAsync);

static const u8 courseSha1s[0x20][0x14] = {
    { 0x1a, 0xe1, 0xa7, 0xd8, 0x94, 0x96, 0x0b, 0x38, 0xe0, 0x9e, 0x74, 0x94, 0x37, 0x33, 0x78,
      0xd8, 0x73, 0x05, 0xa1, 0x63 },
    { 0x90, 0x72, 0x0a, 0x7d, 0x57, 0xa7, 0xc7, 0x6e, 0x23, 0x47, 0x78, 0x2f, 0x6b, 0xde, 0x5d,
      0x22, 0x34, 0x2f, 0xb7, 0xdd },
    { 0x0e, 0x38, 0x03, 0x57, 0xaf, 0xfc, 0xfd, 0x87, 0x22, 0x32, 0x99, 0x94, 0x88, 0x56, 0x99,
      0xd9, 0x92, 0x7f, 0x82, 0x76 },
    { 0x18, 0x96, 0xae, 0xa4, 0x96, 0x17, 0xa5, 0x71, 0xc6, 0x6f, 0xf7, 0x78, 0xd8, 0xf2, 0xab,
      0xbe, 0x9e, 0x5d, 0x74, 0x79 },
    { 0x77, 0x52, 0xbb, 0x51, 0xed, 0xbc, 0x4a, 0x95, 0x37, 0x7c, 0x0a, 0x05, 0xb0, 0xe0, 0xda,
      0x15, 0x03, 0x78, 0x66, 0x25 },
    { 0xe4, 0xbf, 0x36, 0x4c, 0xb0, 0xc5, 0x89, 0x99, 0x07, 0x58, 0x5d, 0x73, 0x16, 0x21, 0xca,
      0x93, 0x0a, 0x4e, 0xf8, 0x5c },
    { 0xb0, 0x2e, 0xd7, 0x2e, 0x00, 0xb4, 0x00, 0x64, 0x7b, 0xda, 0x68, 0x45, 0xbe, 0x38, 0x7c,
      0x47, 0xd2, 0x51, 0xf9, 0xd1 },
    { 0xd1, 0xa4, 0x53, 0xb4, 0x3d, 0x69, 0x20, 0xa7, 0x85, 0x65, 0xe6, 0x5a, 0x45, 0x97, 0xe3,
      0x53, 0xb1, 0x77, 0xab, 0xd0 },
    { 0x72, 0xd0, 0x24, 0x1c, 0x75, 0xbe, 0x4a, 0x5e, 0xbd, 0x24, 0x2b, 0x9d, 0x8d, 0x89, 0xb1,
      0xd6, 0xfd, 0x56, 0xbe, 0x8f },
    { 0x52, 0xf0, 0x1a, 0xe3, 0xae, 0xd1, 0xe0, 0xfa, 0x4c, 0x74, 0x59, 0xa6, 0x48, 0x49, 0x48,
      0x63, 0xe8, 0x3a, 0x54, 0x8c },
    { 0x48, 0xeb, 0xd9, 0xd6, 0x44, 0x13, 0xc2, 0xb9, 0x8d, 0x2b, 0x92, 0xe5, 0xef, 0xc9, 0xb1,
      0x5e, 0xcd, 0x76, 0xfe, 0xe6 },
    { 0xac, 0xc0, 0x88, 0x3a, 0xe0, 0xce, 0x78, 0x79, 0xc6, 0xef, 0xba, 0x20, 0xcf, 0xe5, 0xb5,
      0x90, 0x9b, 0xf7, 0x84, 0x1b },
    { 0x38, 0x48, 0x6c, 0x4f, 0x70, 0x63, 0x95, 0x77, 0x2b, 0xd9, 0x88, 0xc1, 0xac, 0x5f, 0xa3,
      0x0d, 0x27, 0xca, 0xe0, 0x98 },
    { 0xb1, 0x3c, 0x51, 0x54, 0x75, 0xd7, 0xda, 0x20, 0x7d, 0xfd, 0x5b, 0xad, 0xd8, 0x86, 0x98,
      0x61, 0x47, 0xb9, 0x06, 0xff },
    { 0xb9, 0x82, 0x1b, 0x14, 0xa8, 0x93, 0x81, 0xf9, 0xc0, 0x15, 0x66, 0x93, 0x53, 0xcb, 0x24,
      0xd7, 0xdb, 0x1b, 0xb2, 0x5d },
    { 0xff, 0xe5, 0x18, 0x91, 0x5e, 0x5f, 0xaa, 0xa8, 0x89, 0x05, 0x7c, 0x8a, 0x3d, 0x3e, 0x43,
      0x98, 0x68, 0x57, 0x45, 0x08 },
    { 0x80, 0x14, 0x48, 0x8a, 0x60, 0xf4, 0x42, 0x8e, 0xef, 0x52, 0xd0, 0x1f, 0x8c, 0x58, 0x61,
      0xca, 0x95, 0x65, 0xe1, 0xca },
    { 0x8c, 0x85, 0x4b, 0x08, 0x74, 0x17, 0xa9, 0x24, 0x25, 0x11, 0x0c, 0xc7, 0x1e, 0x23, 0xc9,
      0x44, 0xd6, 0x99, 0x78, 0x06 },
    { 0x07, 0x1d, 0x69, 0x7c, 0x4d, 0xdb, 0x66, 0xd3, 0xb2, 0x10, 0xf3, 0x6c, 0x7b, 0xf8, 0x78,
      0x50, 0x2e, 0x79, 0x84, 0x5b },
    { 0x49, 0x51, 0x4e, 0x8f, 0x74, 0xfe, 0xa5, 0x0e, 0x77, 0x27, 0x3c, 0x02, 0x97, 0x08, 0x6d,
      0x67, 0xe5, 0x81, 0x23, 0xe8 },
    { 0xba, 0x9b, 0xcf, 0xb3, 0x73, 0x1a, 0x6c, 0xb1, 0x7d, 0xba, 0x21, 0x9a, 0x8d, 0x37, 0xea,
      0x4d, 0x52, 0x33, 0x22, 0x56 },
    { 0xe8, 0xed, 0x31, 0x60, 0x5c, 0xc7, 0xd6, 0x66, 0x06, 0x91, 0x99, 0x8f, 0x02, 0x4e, 0xed,
      0x6b, 0xa8, 0xb4, 0xa3, 0x3f },
    { 0xbc, 0x03, 0x8e, 0x16, 0x3d, 0x21, 0xd9, 0xa1, 0x18, 0x1b, 0x60, 0xcf, 0x90, 0xb4, 0xd0,
      0x3e, 0xfa, 0xd9, 0xe0, 0xc5 },
    { 0x41, 0x80, 0x99, 0x82, 0x4a, 0xf6, 0xbf, 0x1c, 0xd7, 0xf8, 0xbb, 0x44, 0xf6, 0x1e, 0x3a,
      0x9c, 0xc3, 0x00, 0x7d, 0xae },
    { 0x4e, 0xc5, 0x38, 0x06, 0x5f, 0xdc, 0x8a, 0xcf, 0x49, 0x67, 0x43, 0x00, 0xcb, 0xde, 0xc5,
      0xb8, 0x0c, 0xc0, 0x5a, 0x0d },
    { 0xa4, 0xbe, 0xa4, 0x1b, 0xe8, 0x3d, 0x81, 0x6f, 0x79, 0x3f, 0x3f, 0xad, 0x97, 0xd2, 0x68,
      0xf7, 0x1a, 0xd9, 0x9b, 0xf9 },
    { 0x69, 0x2d, 0x56, 0x6b, 0x05, 0x43, 0x4d, 0x8c, 0x66, 0xa5, 0x5b, 0xdf, 0xf4, 0x86, 0x69,
      0x8e, 0x0f, 0xc9, 0x60, 0x95 },
    { 0x19, 0x41, 0xa2, 0x9a, 0xd2, 0xe7, 0xb7, 0xbb, 0xa8, 0xa2, 0x9e, 0x64, 0x40, 0xc9, 0x5e,
      0xf5, 0xcf, 0x76, 0xb0, 0x1d },
    { 0x07, 0x71, 0x11, 0xb9, 0x96, 0xe5, 0xc4, 0xf4, 0x7d, 0x20, 0xec, 0x29, 0xc2, 0x93, 0x85,
      0x04, 0xb5, 0x3a, 0x8e, 0x76 },
    { 0xf9, 0xa6, 0x2b, 0xef, 0x04, 0xcc, 0x8f, 0x49, 0x96, 0x33, 0xe4, 0x02, 0x3a, 0xcc, 0x76,
      0x75, 0xa9, 0x27, 0x71, 0xf0 },
    { 0xb0, 0x36, 0x86, 0x4c, 0xf0, 0x01, 0x6b, 0xe0, 0x58, 0x14, 0x49, 0xef, 0x29, 0xfb, 0x52,
      0xb2, 0xe5, 0x8d, 0x78, 0xa4 },
    { 0x15, 0xb3, 0x03, 0xb2, 0x88, 0xf4, 0x70, 0x7e, 0x5d, 0x0a, 0xf2, 0x83, 0x67, 0xc8, 0xce,
      0x51, 0xcd, 0xea, 0xb4, 0x90 },
};

static const char *courseAbbreviations[0x20] = {
    "LC",
    "MMM",
    "MG",
    "TF",
    "MC",
    "CM",
    "DKSC",
    "WGM",
    "DC",
    "KC",
    "MT",
    "GV",
    "DDR",
    "MH",
    "BC",
    "RR",
    "rPB",
    "rYF",
    "rGV2",
    "rMR",
    "rSL",
    "rSGB",
    "rDS",
    "rWS",
    "rDH",
    "rBC3",
    "rDKJP",
    "rMC",
    "rMC3",
    "rPG",
    "rDKM",
    "rBC",
};

static char nibbleToChar(u8 val) {
    if (val < 0xa) {
        return '0' + val;
    }
    return 'a' + val - 0xa;
}

static void getCourseName(const u8 *courseSha1, char *courseName) {
    for (u32 i = 0; i < 0x20; i++) {
        if (!memcmp(courseSha1, courseSha1s[i], 0x14)) {
            memcpy(courseName, courseAbbreviations[i], strlen(courseAbbreviations[i]) + 1);
            return;
        }
    }

    for (u32 i = 0; i < 2 * 0x14; i += 2) {
        courseName[i] = nibbleToChar(courseSha1[i / 2] >> 4);
        courseName[i + 1] = nibbleToChar(courseSha1[i / 2] & 0xf);
    }
    courseName[2 * 0x14] = '\0';
}

static void SaveManager_saveGhost(SaveManager *this, GhostFile *file) {
    OSJoinThread(&this->ghostInitThread, NULL);
    OSDetachThread(&this->ghostInitThread);

    this->saveGhostResult = false;

    if (file->raceTime.minutes > 99) {
        goto fail;
    }

    u32 size = GhostFile_spWrite(file, this->rawGhostFile);
    if (size == 0) {
        goto fail;
    }

    wchar_t path[255 + 1];
    u32 offset = swprintf(path, 255 + 1, L"/mkw-sp/ghosts/");

    char courseName[0x14 * 2 + 1];
    getCourseName(this->courseSha1s[file->courseId], courseName);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%s", courseName);

    if (!Storage_createDir(path, true)) {
        goto fail;
    }

    offset += swprintf(path + offset, 255 + 1 - offset, L"/%02um", file->raceTime.minutes);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%02us", file->raceTime.seconds);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%03u ", file->raceTime.milliseconds);
    const wchar_t *miiName = file->rawMii.name;
    if (miiName[0] == L'\0') {
        miiName = L"Player";
    }
    offset += swprintf(path + offset, 255 + 1 - offset, L"%ls", miiName);
    swprintf(path + offset, 255 + 1 - offset, L".rkg");

    if (!Storage_writeFile(path, false, this->rawGhostFile, size)) {
        u32 i;
        for (i = 0; i < 100; i++) {
            swprintf(path + offset, 255 + 1 - offset, L" %u.rkg", i);
            if (Storage_writeFile(path, false, this->rawGhostFile, size)) {
                break;
            }
        }
        if (i >= 100) {
            goto fail;
        }
    }

    this->saveGhostResult = true;

    NodeInfo info;
    Storage_stat(path, &info);
    if (info.type == NODE_TYPE_FILE) {
        SaveManager_initGhost(this, info.id);
    }

fail:
    this->isBusy = false;
}

static void saveGhostTask(void *arg) {
    GhostFile *file = arg;
    SaveManager_saveGhost(s_saveManager, file);
}

static void my_SaveManager_saveGhostAsync(SaveManager *this, s32 UNUSED(licenseId),
        u32 UNUSED(category), u32 UNUSED(index), GhostFile *file, bool UNUSED(saveLicense)) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, saveGhostTask, file, NULL);
}
PATCH_B(SaveManager_saveGhostAsync, my_SaveManager_saveGhostAsync);

static void SaveManager_computeCourseSha1(SaveManager *this, u32 courseId) {
    ResourceManager_preloadCourseAsync(s_resourceManager, courseId);
    ResourceManager_process(s_resourceManager);

    NETSHA1Context cx;
    NETSHA1Init(&cx);
    const DvdArchive *archive = &s_resourceManager->courseCache.multi->archives[0];
    NETSHA1Update(&cx, archive->archiveBuffer, archive->archiveSize);
    NETSHA1GetDigest(&cx, this->courseSha1s[courseId]);
    this->courseSha1IsValid[courseId] = true;

    this->isBusy = false;
}

static void computeCourseSha1Task(void *arg) {
    u32 courseId = (u32)arg;
    SaveManager_computeCourseSha1(s_saveManager, courseId);
}

bool SaveManager_computeCourseSha1Async(SaveManager *this, u32 courseId) {
    if (this->courseSha1IsValid[courseId]) {
        return true;
    }

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, computeCourseSha1Task, (void *)courseId, NULL);
    return false;
}

const u8 *SaveManager_getCourseSha1(const SaveManager *this, u32 courseId) {
    return this->courseSha1s[courseId];
}

bool vsSpeedModIsEnabled;
