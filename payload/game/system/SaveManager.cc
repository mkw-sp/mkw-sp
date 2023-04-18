#include "SaveManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/ResourceManager.hh"
#include "game/system/RootScene.hh"
extern "C" {
#include "game/system/SaveManager.h"
}
#include "game/ui/SectionManager.hh"

#include <common/Bytes.hh>

#include <bit>
#include <cstring>

namespace System {

void SaveManager::RawLicense::reset() {
    *this = {};
    unlockFlags[0] = 0xffffffff;
    unlockFlags[1] = 0x31ffffc;
    rules[0] = 0x5008;
    rules[1] = 0x1000;
    rules[2] = 0x5008;
    rules[3] = 0x1000;
    vr = 5000;
    br = 5000;
    auto value = SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::DriftMode>();
    driftMode = (static_cast<u32>(value) + 1) << 14;
}

SaveManager *SaveManager::CreateInstance() {
    auto *heap = RootScene::Instance()->m_heapCollection.mem1;

    s_instance = new (heap, 0x4) SaveManager;
    assert(s_instance);

    s_instance->m_ghostCount = 0;
    s_instance->m_rawGhostHeaders = new (heap, 0x4) RawGhostHeader[MAX_GHOST_COUNT];
    s_instance->m_ghostFooters = new (heap, 0x4) GhostFooter[MAX_GHOST_COUNT];
    s_instance->m_ghostIds = new (heap, 0x4) SP::Storage::NodeId[MAX_GHOST_COUNT];

    s_instance->m_spCanSave = true;
    s_instance->m_spLicenseCount = 0;
    for (u32 i = 0; i < std::size(s_instance->m_spLicenses); i++) {
        s_instance->m_spLicenses[i] = SP::ClientSettings::Settings();
        s_instance->m_spLicenses[i].reset();
    }
    s_instance->m_spCurrentLicense = {};

    return s_instance;
}

void SaveManager::initAsync() {
    m_isBusy = true;
    m_taskThread->request(InitTask, nullptr, nullptr);
}

void SaveManager::InitTask(void * /* arg */) {
    assert(s_instance);
    s_instance->init();
}

void SaveManager::init() {
    m_isValid = true;
    m_canSave = false;

    m_otherRawSave = m_rawSave;

    initSPSave();
    initGhostsAsync();

    m_isBusy = false;
}

void SaveManager::initSPSave() {
    // TODO: Hopefully this is enough. Can always stream the file if not.
    char iniBuffer[2048];

    for (m_spLicenseCount = 0; m_spLicenseCount < std::size(m_spLicenses);) {
        wchar_t path[64];
        swprintf(path, std::size(path), L"/mkw-sp/settings%u.ini", m_spLicenseCount);

        auto size = SP::Storage::ReadFile(path, iniBuffer, sizeof(iniBuffer));
        if (!size) {
            break;
        }

        m_spLicenses[m_spLicenseCount++].readIni(iniBuffer, *size);
    }
}

void SaveManager::initGhostsAsync() {
    u8 *stackTop = m_ghostInitStack + sizeof(m_ghostInitStack);
    u32 stackSize = sizeof(m_ghostInitStack);
    OSCreateThread(&m_ghostInitThread, InitGhostsTask, nullptr, stackTop, stackSize, 31, 0);
    OSResumeThread(&m_ghostInitThread);
}

void *SaveManager::InitGhostsTask(void * /* arg */) {
    assert(s_instance);
    s_instance->initGhosts();
    return nullptr;
}

void SaveManager::initGhosts() {
    SP_LOG("Initializing ghosts...");

    initGhosts(L"/mkw-sp/ghosts");
    initGhosts(L"/ctgpr/ghosts");

    SP::Storage::CreateDir(L"/mkw-sp/ghosts", true);

    SP_LOG("Ghosts: %u / %u", m_ghostCount, MAX_GHOST_COUNT);
}

void SaveManager::initGhosts(const wchar_t *path) {
    auto info = SP::Storage::Stat(path);
    if (!info) {
        return;
    }
    if (info->type != SP::Storage::NodeType::Dir) {
        return;
    }
    initGhosts(info->id);
}

void SaveManager::initGhosts(SP::Storage::NodeId id) {
    if (m_ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    auto dir = SP::Storage::FastOpenDir(id);
    if (!dir) {
        return;
    }

    while (auto info = dir->read()) {
        if (info->type == SP::Storage::NodeType::Dir) {
            initGhosts(info->id);
        } else {
            initGhost(info->id);
        }
    }
}

void SaveManager::initGhost(SP::Storage::NodeId id) {
    if (m_ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    auto readSize = SP::Storage::FastReadFile(id, m_rawGhostFile, 0x2800);
    if (!readSize) {
        return;
    }

    if (!RawGhostFile::IsValid(m_rawGhostFile, *readSize)) {
        return;
    }

    auto *header = reinterpret_cast<const RawGhostHeader *>(m_rawGhostFile);
    memcpy(&m_rawGhostHeaders[m_ghostCount], header, sizeof(RawGhostHeader));
    m_ghostFooters[m_ghostCount] = GhostFooter(m_rawGhostFile, *readSize);
    m_ghostIds[m_ghostCount] = id;
    m_ghostCount++;
}

void SaveManager::resetAsync() {
    m_isValid = true;
    m_canSave = false;

    m_isBusy = false;
    m_result = NandResult::Ok;
}

bool SaveManager::isBusy() const {
    return m_isBusy;
}

bool SaveManager::saveGhostResult() const {
    return m_saveGhostResult;
}

void SaveManager::saveLicensesAsync() {
    if (m_spCurrentLicense) {
        auto *context = UI::SectionManager::Instance()->globalContext();

        Registry::Character character = context->m_localCharacterIds[0];
        Registry::Vehicle vehicle = context->m_localVehicleIds[0];
        setSetting<SP::ClientSettings::Setting::Character>(character);
        setSetting<SP::ClientSettings::Setting::Vehicle>(vehicle);

        auto driftMode = static_cast<SP::ClientSettings::DriftMode>(context->m_driftModes[0] - 1);
        setSetting<SP::ClientSettings::Setting::DriftMode>(driftMode);
    }

    if (!m_spCanSave) {
        m_isBusy = false;
        m_result = NandResult::Ok;
    }

    m_isBusy = true;
    m_taskThread->request(SaveSPSaveTask, nullptr, nullptr);
}

void SaveManager::SaveSPSaveTask(void * /* arg */) {
    assert(s_instance);
    s_instance->saveSPSave();
}

void SaveManager::saveSPSave() {
    char iniBuffer[2048];

    for (size_t i = 0; i < m_spLicenseCount; ++i) {
        m_spLicenses[i].writeIni(iniBuffer, sizeof(iniBuffer));

        wchar_t path[64];
        swprintf(path, std::size(path), L"/mkw-sp/settings%u.ini", (unsigned)i);

        if (!SP::Storage::WriteFile(path, iniBuffer, strlen(iniBuffer), true)) {
            SP_LOG("Failed to save %ls", path);
            m_spCanSave = false;
            m_isBusy = false;
            m_result = NandResult::NoSpace;
            return;
        }
    }

    for (size_t i = m_spLicenseCount; i < 6; ++i) {
        wchar_t path[64];
        swprintf(path, std::size(path), L"/mkw-sp/settings%u.ini", (unsigned)i);

        if (!SP::Storage::Remove(path, true)) {
            SP_LOG("Failed to delete %ls", path);
            m_spCanSave = false;
            m_isBusy = false;
            m_result = NandResult::NoSpace;
            return;
        }
    }

    m_isBusy = false;
    m_result = NandResult::Ok;
}

void SaveManager::selectLicense(u32 licenseId) {
    m_currentLicenseId = licenseId;
}

void SaveManager::refreshGCPadRumble() {
    auto setting = getSetting<SP::ClientSettings::Setting::GCPadRumble>();
    m_rawSave->m_rumble = setting == SP::ClientSettings::GCPadRumble::Enable;
}

void SaveManager::refreshRegionFlagDisplay() {
    auto setting = getSetting<SP::ClientSettings::Setting::RegionFlagDisplay>();
    m_rawSave->m_flagDisplay = setting == SP::ClientSettings::RegionFlagDisplay::Enable;
}

u32 SaveManager::spLicenseCount() const {
    return m_spLicenseCount;
}

void SaveManager::eraseSPLicense() {
    if (!m_spCurrentLicense) {
        return;
    }
    for (u32 i = *m_spCurrentLicense; i < m_spLicenseCount - 1; i++) {
        m_spLicenses[i] = m_spLicenses[i + 1];
    }
    m_spLicenseCount--;
    m_spCurrentLicense.reset();
}

void SaveManager::createSPLicense(const MiiId *miiId) {
    SP::ClientSettings::Settings &license = m_spLicenses[m_spLicenseCount++];
    license.reset();
    m_spCurrentLicense = m_spLicenseCount - 1;
    setMiiId(*miiId);
}

std::optional<u32> SaveManager::spCurrentLicense() const {
    return m_spCurrentLicense;
}

void SaveManager::selectSPLicense(u32 licenseId) {
    m_spCurrentLicense = licenseId;

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

void SaveManager::unselectSPLicense() {
    m_spCurrentLicense.reset();

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

u32 SaveManager::getSetting(u32 setting) const {
    if (!m_spCurrentLicense) {
        return SP::ClientSettings::entries[setting].defaultValue;
    }

    auto vanillaValue = SP::ClientSettings::entries[setting].vanillaValue;
    if (vanillaValue) {
        auto vanillaModeSetting = getSetting<SP::ClientSettings::Setting::VanillaMode>();
        if (vanillaModeSetting == SP::ClientSettings::VanillaMode::Enable) {
            return *vanillaValue;
        }
    }

    return m_spLicenses[*m_spCurrentLicense].get(setting);
}

void SaveManager::setSetting(u32 setting, u32 value) {
    if (!m_spCurrentLicense) {
        return;
    }

    m_spLicenses[*m_spCurrentLicense].set(setting, value);

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

void SaveManager::setSetting(const char *key, const char *value) {
    if (!m_spCurrentLicense) {
        return;
    }

    m_spLicenses[*m_spCurrentLicense].set(key, value);

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

void SaveManager::setMiiId(MiiId miiId) {
    u32 avatar = Bytes::Read<u32>(miiId.avatar, 0);
    u32 client = Bytes::Read<u32>(miiId.client, 0);
    SP::ClientSettings::Settings &license = m_spLicenses[*m_spCurrentLicense];
    license.set<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiAvatar>(avatar);
    license.set<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiClient>(client);
}

System::MiiId SaveManager::getMiiId(u32 licenseId) const {
    const SP::ClientSettings::Settings &license = m_spLicenses[licenseId];
    u32 avatar = license.get<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiAvatar>();
    u32 client = license.get<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiClient>();
    MiiId miiId{};
    Bytes::Write<u32>(miiId.avatar, 0, avatar);
    Bytes::Write<u32>(miiId.client, 0, client);
    return miiId;
}

u32 SaveManager::ghostCount() const {
    return m_ghostCount;
}

RawGhostHeader *SaveManager::rawGhostHeader(u32 i) {
    return &m_rawGhostHeaders[i];
}

GhostFooter *SaveManager::ghostFooter(u32 i) {
    return &m_ghostFooters[i];
}

void SaveManager::loadGhostHeadersAsync(s32 /* licenseId */, GhostGroup * /* group */) {
    m_isBusy = true;
    m_taskThread->request(LoadGhostHeadersTask, nullptr, nullptr);
}

void SaveManager::LoadGhostHeadersTask(void * /* arg */) {
    assert(s_instance);
    s_instance->loadGhostHeaders();
}

void SaveManager::loadGhostHeaders() {
    if (OSJoinThread(&m_ghostInitThread, nullptr)) {
        OSDetachThread(&m_ghostInitThread);
    }

    m_isBusy = false;
    m_result = NandResult::Ok;
}

void SaveManager::loadGhostAsync(s32 /* licenseId */, u32 /* category */, u32 /* index */,
        u32 /* courseId */) {
    m_isBusy = true;
    m_taskThread->request(LoadGhostsTask, nullptr, nullptr);
}

void SaveManager::LoadGhostsTask(void * /* arg */) {
    assert(s_instance);
    s_instance->loadGhosts();
}

void SaveManager::loadGhosts() {
    if (OSJoinThread(&m_ghostInitThread, nullptr)) {
        OSDetachThread(&m_ghostInitThread);
    }

    auto *raceConfig = RaceConfig::Instance();
    auto &raceScenario = raceConfig->raceScenario();
    auto &menuScenario = raceConfig->menuScenario();
    if (menuScenario.ghostBuffer == raceScenario.ghostBuffer) {
        if (menuScenario.ghostBuffer == raceConfig->ghostBuffers() + 0) {
            menuScenario.ghostBuffer = raceConfig->ghostBuffers() + 1;
        } else {
            menuScenario.ghostBuffer = raceConfig->ghostBuffers() + 0;
        }
    }

    auto *context = UI::SectionManager::Instance()->globalContext();
    for (u32 i = 0; i < context->m_timeAttackGhostCount; i++) {
        if (!loadGhost(i)) {
            memset((*menuScenario.ghostBuffer)[i], 0, 0x2800);
        }
    }

    m_isBusy = false;
}

bool SaveManager::loadGhost(u32 i) {
    auto *context = UI::SectionManager::Instance()->globalContext();
    auto &menuScenario = RaceConfig::Instance()->menuScenario();

    auto id = m_ghostIds[context->m_timeAttackGhostIndices[i]];
    auto readSize = SP::Storage::FastReadFile(id, m_rawGhostFile, 0x2800);
    if (!readSize) {
        return false;
    }

    if (reinterpret_cast<RawGhostHeader *>(m_rawGhostFile)->isCompressed) {
        if (!RawGhostFile::IsValid(m_rawGhostFile, *readSize)) {
            return false;
        }

        if (!RawGhostFile::Decompress(m_rawGhostFile, (*menuScenario.ghostBuffer)[i])) {
            return false;
        }
    } else {
        memcpy((*menuScenario.ghostBuffer)[i], m_rawGhostFile, 0x2800);
    }

    return RawGhostFile::IsValid((*menuScenario.ghostBuffer)[i], 0x2800);
}

void SaveManager::saveGhostAsync(s32 /* licenseId */, u32 /* category */, u32 /* index */,
        GhostFile *file, bool /* saveLicense */) {
    m_isBusy = true;
    m_taskThread->request(SaveGhostTask, file, nullptr);
}

void SaveManager::SaveGhostTask(void *arg) {
    assert(s_instance);
    auto file = reinterpret_cast<GhostFile *>(arg);
    s_instance->saveGhost(file);
    s_instance->m_isBusy = false;
}

void SaveManager::saveGhost(GhostFile *file) {
    if (OSJoinThread(&m_ghostInitThread, nullptr)) {
        OSDetachThread(&m_ghostInitThread);
    }

    m_saveGhostResult = false;

    u8 courseSHA1[0x14];
    ResourceManager::ComputeCourseSHA1(courseSHA1);
    SPFooter::OnRaceEnd(courseSHA1);

    Time raceTime = file->raceTime();
    if (raceTime.minutes > 99) {
        return;
    }

    auto size = file->write(m_rawGhostFile);
    if (!size) {
        return;
    }

    wchar_t path[255 + 1];
    u32 offset = swprintf(path, 255 + 1, L"/mkw-sp/ghosts/");

    char courseName[0x14 * 2 + 1];
    GetCourseName(courseSHA1, courseName);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%s", courseName);

    if (!SP::Storage::CreateDir(path, true)) {
        return;
    }

    offset += swprintf(path + offset, 255 + 1 - offset, L"/%02um", raceTime.minutes);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%02us", raceTime.seconds);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%03u ", raceTime.milliseconds);
    RawMii mii = file->rawMii();
    const wchar_t *miiName = mii.name;
    if (miiName[0] == L'\0') {
        miiName = L"Player";
    }
    offset += swprintf(path + offset, 255 + 1 - offset, L"%ls", miiName);
    swprintf(path + offset, 255 + 1 - offset, L".rkg");

    if (!SP::Storage::WriteFile(path, m_rawGhostFile, *size, false)) {
        u32 i;
        for (i = 0; i < 100; i++) {
            swprintf(path + offset, 255 + 1 - offset, L" %u.rkg", i);
            if (SP::Storage::WriteFile(path, m_rawGhostFile, *size, false)) {
                break;
            }
        }
        if (i >= 100) {
            return;
        }
    }

    m_saveGhostResult = true;

    auto info = SP::Storage::Stat(path);
    if (info && info->type == SP::Storage::NodeType::File) {
        initGhost(info->id);
    }
}

void SaveManager::GetCourseName(const u8 *courseSHA1, char (&courseName)[0x14 * 2 + 1]) {
    for (u32 i = 0; i < 0x20; i++) {
        if (!memcmp(courseSHA1, s_courseSHA1s[i], 0x14)) {
            snprintf(courseName, std::size(courseName), "%s", s_courseAbbreviations[i]);
            return;
        }
    }

    for (u32 i = 0, offset = 0; i < 0x14; i++) {
        offset += snprintf(courseName, std::size(courseName) - offset, "%02x", courseSHA1[i]);
    }
}

SaveManager *SaveManager::Instance() {
    return s_instance;
}

const u8 SaveManager::s_courseSHA1s[32][0x14] = {
        {0x1a, 0xe1, 0xa7, 0xd8, 0x94, 0x96, 0x0b, 0x38, 0xe0, 0x9e, 0x74, 0x94, 0x37, 0x33, 0x78,
                0xd8, 0x73, 0x05, 0xa1, 0x63},
        {0x90, 0x72, 0x0a, 0x7d, 0x57, 0xa7, 0xc7, 0x6e, 0x23, 0x47, 0x78, 0x2f, 0x6b, 0xde, 0x5d,
                0x22, 0x34, 0x2f, 0xb7, 0xdd},
        {0x0e, 0x38, 0x03, 0x57, 0xaf, 0xfc, 0xfd, 0x87, 0x22, 0x32, 0x99, 0x94, 0x88, 0x56, 0x99,
                0xd9, 0x92, 0x7f, 0x82, 0x76},
        {0x18, 0x96, 0xae, 0xa4, 0x96, 0x17, 0xa5, 0x71, 0xc6, 0x6f, 0xf7, 0x78, 0xd8, 0xf2, 0xab,
                0xbe, 0x9e, 0x5d, 0x74, 0x79},
        {0x77, 0x52, 0xbb, 0x51, 0xed, 0xbc, 0x4a, 0x95, 0x37, 0x7c, 0x0a, 0x05, 0xb0, 0xe0, 0xda,
                0x15, 0x03, 0x78, 0x66, 0x25},
        {0xe4, 0xbf, 0x36, 0x4c, 0xb0, 0xc5, 0x89, 0x99, 0x07, 0x58, 0x5d, 0x73, 0x16, 0x21, 0xca,
                0x93, 0x0a, 0x4e, 0xf8, 0x5c},
        {0xb0, 0x2e, 0xd7, 0x2e, 0x00, 0xb4, 0x00, 0x64, 0x7b, 0xda, 0x68, 0x45, 0xbe, 0x38, 0x7c,
                0x47, 0xd2, 0x51, 0xf9, 0xd1},
        {0xd1, 0xa4, 0x53, 0xb4, 0x3d, 0x69, 0x20, 0xa7, 0x85, 0x65, 0xe6, 0x5a, 0x45, 0x97, 0xe3,
                0x53, 0xb1, 0x77, 0xab, 0xd0},
        {0x72, 0xd0, 0x24, 0x1c, 0x75, 0xbe, 0x4a, 0x5e, 0xbd, 0x24, 0x2b, 0x9d, 0x8d, 0x89, 0xb1,
                0xd6, 0xfd, 0x56, 0xbe, 0x8f},
        {0x52, 0xf0, 0x1a, 0xe3, 0xae, 0xd1, 0xe0, 0xfa, 0x4c, 0x74, 0x59, 0xa6, 0x48, 0x49, 0x48,
                0x63, 0xe8, 0x3a, 0x54, 0x8c},
        {0x48, 0xeb, 0xd9, 0xd6, 0x44, 0x13, 0xc2, 0xb9, 0x8d, 0x2b, 0x92, 0xe5, 0xef, 0xc9, 0xb1,
                0x5e, 0xcd, 0x76, 0xfe, 0xe6},
        {0xac, 0xc0, 0x88, 0x3a, 0xe0, 0xce, 0x78, 0x79, 0xc6, 0xef, 0xba, 0x20, 0xcf, 0xe5, 0xb5,
                0x90, 0x9b, 0xf7, 0x84, 0x1b},
        {0x38, 0x48, 0x6c, 0x4f, 0x70, 0x63, 0x95, 0x77, 0x2b, 0xd9, 0x88, 0xc1, 0xac, 0x5f, 0xa3,
                0x0d, 0x27, 0xca, 0xe0, 0x98},
        {0xb1, 0x3c, 0x51, 0x54, 0x75, 0xd7, 0xda, 0x20, 0x7d, 0xfd, 0x5b, 0xad, 0xd8, 0x86, 0x98,
                0x61, 0x47, 0xb9, 0x06, 0xff},
        {0xb9, 0x82, 0x1b, 0x14, 0xa8, 0x93, 0x81, 0xf9, 0xc0, 0x15, 0x66, 0x93, 0x53, 0xcb, 0x24,
                0xd7, 0xdb, 0x1b, 0xb2, 0x5d},
        {0xff, 0xe5, 0x18, 0x91, 0x5e, 0x5f, 0xaa, 0xa8, 0x89, 0x05, 0x7c, 0x8a, 0x3d, 0x3e, 0x43,
                0x98, 0x68, 0x57, 0x45, 0x08},
        {0x80, 0x14, 0x48, 0x8a, 0x60, 0xf4, 0x42, 0x8e, 0xef, 0x52, 0xd0, 0x1f, 0x8c, 0x58, 0x61,
                0xca, 0x95, 0x65, 0xe1, 0xca},
        {0x8c, 0x85, 0x4b, 0x08, 0x74, 0x17, 0xa9, 0x24, 0x25, 0x11, 0x0c, 0xc7, 0x1e, 0x23, 0xc9,
                0x44, 0xd6, 0x99, 0x78, 0x06},
        {0x07, 0x1d, 0x69, 0x7c, 0x4d, 0xdb, 0x66, 0xd3, 0xb2, 0x10, 0xf3, 0x6c, 0x7b, 0xf8, 0x78,
                0x50, 0x2e, 0x79, 0x84, 0x5b},
        {0x49, 0x51, 0x4e, 0x8f, 0x74, 0xfe, 0xa5, 0x0e, 0x77, 0x27, 0x3c, 0x02, 0x97, 0x08, 0x6d,
                0x67, 0xe5, 0x81, 0x23, 0xe8},
        {0xba, 0x9b, 0xcf, 0xb3, 0x73, 0x1a, 0x6c, 0xb1, 0x7d, 0xba, 0x21, 0x9a, 0x8d, 0x37, 0xea,
                0x4d, 0x52, 0x33, 0x22, 0x56},
        {0xe8, 0xed, 0x31, 0x60, 0x5c, 0xc7, 0xd6, 0x66, 0x06, 0x91, 0x99, 0x8f, 0x02, 0x4e, 0xed,
                0x6b, 0xa8, 0xb4, 0xa3, 0x3f},
        {0xbc, 0x03, 0x8e, 0x16, 0x3d, 0x21, 0xd9, 0xa1, 0x18, 0x1b, 0x60, 0xcf, 0x90, 0xb4, 0xd0,
                0x3e, 0xfa, 0xd9, 0xe0, 0xc5},
        {0x41, 0x80, 0x99, 0x82, 0x4a, 0xf6, 0xbf, 0x1c, 0xd7, 0xf8, 0xbb, 0x44, 0xf6, 0x1e, 0x3a,
                0x9c, 0xc3, 0x00, 0x7d, 0xae},
        {0x4e, 0xc5, 0x38, 0x06, 0x5f, 0xdc, 0x8a, 0xcf, 0x49, 0x67, 0x43, 0x00, 0xcb, 0xde, 0xc5,
                0xb8, 0x0c, 0xc0, 0x5a, 0x0d},
        {0xa4, 0xbe, 0xa4, 0x1b, 0xe8, 0x3d, 0x81, 0x6f, 0x79, 0x3f, 0x3f, 0xad, 0x97, 0xd2, 0x68,
                0xf7, 0x1a, 0xd9, 0x9b, 0xf9},
        {0x69, 0x2d, 0x56, 0x6b, 0x05, 0x43, 0x4d, 0x8c, 0x66, 0xa5, 0x5b, 0xdf, 0xf4, 0x86, 0x69,
                0x8e, 0x0f, 0xc9, 0x60, 0x95},
        {0x19, 0x41, 0xa2, 0x9a, 0xd2, 0xe7, 0xb7, 0xbb, 0xa8, 0xa2, 0x9e, 0x64, 0x40, 0xc9, 0x5e,
                0xf5, 0xcf, 0x76, 0xb0, 0x1d},
        {0x07, 0x71, 0x11, 0xb9, 0x96, 0xe5, 0xc4, 0xf4, 0x7d, 0x20, 0xec, 0x29, 0xc2, 0x93, 0x85,
                0x04, 0xb5, 0x3a, 0x8e, 0x76},
        {0xf9, 0xa6, 0x2b, 0xef, 0x04, 0xcc, 0x8f, 0x49, 0x96, 0x33, 0xe4, 0x02, 0x3a, 0xcc, 0x76,
                0x75, 0xa9, 0x27, 0x71, 0xf0},
        {0xb0, 0x36, 0x86, 0x4c, 0xf0, 0x01, 0x6b, 0xe0, 0x58, 0x14, 0x49, 0xef, 0x29, 0xfb, 0x52,
                0xb2, 0xe5, 0x8d, 0x78, 0xa4},
        {0x15, 0xb3, 0x03, 0xb2, 0x88, 0xf4, 0x70, 0x7e, 0x5d, 0x0a, 0xf2, 0x83, 0x67, 0xc8, 0xce,
                0x51, 0xcd, 0xea, 0xb4, 0x90},
};

const char *SaveManager::s_courseAbbreviations[0x20] = {
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

} // namespace System

extern "C" {
bool SaveManager_SaveGhostResult(void) {
    return System::SaveManager::Instance()->saveGhostResult();
}

void SaveManager_EraseLicense(u32 licenseId) {
    return System::SaveManager::Instance()->eraseLicense(licenseId);
}

u32 SaveManager_SPLicenseCount(void) {
    return System::SaveManager::Instance()->spLicenseCount();
}

void SaveManager_EraseSPLicense(void) {
    System::SaveManager::Instance()->eraseSPLicense();
}

void SaveManager_CreateSPLicense(const MiiId *miiId) {
    System::SaveManager::Instance()->createSPLicense(
            reinterpret_cast<const System::MiiId *>(miiId));
}

s32 SaveManager_SPCurrentLicense(void) {
    return System::SaveManager::Instance()->spCurrentLicense().value_or(-1);
}

u32 SaveManager_GetVanillaMode(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetFOV169(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::FOV169>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetMapIcons(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::MapIcons>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTAClass(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TAClass>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTAGhostTagVisibility(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TAGhostTagVisibility>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTASolidGhosts(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TASolidGhosts>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTAGhostSound(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TAGhostSound>();
    return static_cast<u32>(value);
}

void SaveManager_SetMiiId(const MiiId *miiId) {
    auto *saveManager = System::SaveManager::Instance();
    saveManager->setMiiId(*std::bit_cast<System::MiiId *>(miiId));
}

MiiId SaveManager_GetSPLicenseMiiId(u32 licenseId) {
    auto *saveManager = System::SaveManager::Instance();
    return std::bit_cast<MiiId>(saveManager->getMiiId(licenseId));
}

bool vsSpeedModIsEnabled;
}
