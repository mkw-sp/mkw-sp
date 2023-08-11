#include "SaveManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/ResourceManager.hh"
#include "game/system/RootScene.hh"
#include "game/ui/SectionManager.hh"

#include <common/Bytes.hh>
#include <sp/storage/DecompLoader.hh>

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
    initCourseSHA1s();
    initGhostsAsync();

    m_isBusy = false;
}

void SaveManager::initSPSave() {
    // TODO: Hopefully this is enough. Can always stream the file if not.
    char iniBuffer[2048];

    SP::Storage::CreateDir(L"/mkw-sp/licenses", true);
    for (m_spLicenseCount = 0; m_spLicenseCount < std::size(m_spLicenses);) {
        wchar_t path[64], pathOld[64];
        swprintf(pathOld, std::size(pathOld), L"/mkw-sp/settings%u.ini", m_spLicenseCount);
        swprintf(path, std::size(path), L"/mkw-sp/licenses/slot%u.ini", m_spLicenseCount);

        bool oldLicenseExists = static_cast<bool>(SP::Storage::Open(pathOld, "r"));

        if (oldLicenseExists) {
            SP::Storage::Rename(pathOld, path);
        }
        auto size = SP::Storage::ReadFile(path, iniBuffer, sizeof(iniBuffer));
        if (!size) {
            break;
        }

        m_spLicenses[m_spLicenseCount++].readIni(iniBuffer, *size);
    }
}

void SaveManager::initCourseSHA1s() {
    for (u8 courseId = 0; courseId < m_courseSHA1s.size(); courseId++) {
        char path[128];
        snprintf(path, sizeof(path), "Race/Course/%s.szs",
                ResourceManager::CourseFilenames[courseId]);
        u8 *buffer;
        size_t size;
        auto *heap = RootScene::Instance()->m_heapCollection.mem2;
        if (!SP::Storage::DecompLoader::LoadRO(path, &buffer, &size, heap,
                    SP::Storage::StorageType::FAT)) {
            m_courseSHA1s[courseId] = s_courseSHA1s[courseId];
            continue;
        }

        SP_LOG("Hashing course %s", path);
        NETSHA1Context context;
        NETSHA1Init(&context);
        NETSHA1Update(&context, buffer, size);
        NETSHA1GetDigest(&context, m_courseSHA1s[courseId].data());
        delete[] buffer;
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
        swprintf(path, std::size(path), L"/mkw-sp/licenses/slot%u.ini", (unsigned)i);

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
        swprintf(path, std::size(path), L"/mkw-sp/licenses/slot%u.ini", (unsigned)i);

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

    if (static_cast<SP::ClientSettings::Setting>(setting) == SP::ClientSettings::Setting::YButton &&
            static_cast<SP::ClientSettings::YButton>(value) ==
                    SP::ClientSettings::YButton::ItemWheel) {
        m_usedItemWheel = true;
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

    if (m_usedItemWheel) {
        return;
    }

    m_saveGhostResult = false;

    Sha1 courseSha1;
    auto &raceScenario = System::RaceConfig::Instance()->m_spRace;
    if (raceScenario.courseSha.has_value()) {
        courseSha1 = *raceScenario.courseSha;
    } else {
        courseSha1 = m_courseSHA1s[file->courseId()];
    }

    SPFooter::OnRaceEnd(courseSha1);

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
    GetCourseName(courseSha1, courseName);
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

void SaveManager::GetCourseName(Sha1 courseSHA1, char (&courseName)[0x14 * 2 + 1]) {
    for (u32 i = 0; i < 0x20; i++) {
        if (courseSHA1 == s_courseSHA1s[i]) {
            snprintf(courseName, std::size(courseName), "%s", s_courseAbbreviations[i]);
            return;
        }
    }

    hydro_bin2hex(courseName, std::size(courseName), courseSHA1.data(), courseSHA1.size());
}

Sha1 SaveManager::courseSHA1(Registry::Course courseId) const {
    return m_courseSHA1s[static_cast<u32>(courseId)];
}

Sha1 SaveManager::vanillaSHA1(Registry::Course courseId) const {
    return s_courseSHA1s[static_cast<u32>(courseId)];
}

bool SaveManager::isCourseReplaced(Registry::Course courseId) const {
    return courseSHA1(courseId) != vanillaSHA1(courseId);
}

SaveManager *SaveManager::Instance() {
    return s_instance;
}

const std::array<Sha1, 42> SaveManager::s_courseSHA1s = {
        *sha1FromHex("7752bb51edbc4a95377c0a05b0e0da1503786625"),
        *sha1FromHex("90720a7d57a7c76e2347782f6bde5d22342fb7dd"),
        *sha1FromHex("0e380357affcfd8722329994885699d9927f8276"),
        *sha1FromHex("acc0883ae0ce7879c6efba20cfe5b5909bf7841b"),
        *sha1FromHex("1896aea49617a571c66ff778d8f2abbe9e5d7479"),
        *sha1FromHex("e4bf364cb0c5899907585d731621ca930a4ef85c"),
        *sha1FromHex("b02ed72e00b400647bda6845be387c47d251f9d1"),
        *sha1FromHex("d1a453b43d6920a78565e65a4597e353b177abd0"),
        *sha1FromHex("1ae1a7d894960b38e09e7494373378d87305a163"),
        *sha1FromHex("72d0241c75be4a5ebd242b9d8d89b1d6fd56be8f"),
        *sha1FromHex("b13c515475d7da207dfd5badd886986147b906ff"),
        *sha1FromHex("48ebd9d64413c2b98d2b92e5efc9b15ecd76fee6"),
        *sha1FromHex("b9821b14a89381f9c015669353cb24d7db1bb25d"),
        *sha1FromHex("ffe518915e5faaa889057c8a3d3e439868574508"),
        *sha1FromHex("38486c4f706395772bd988c1ac5fa30d27cae098"),
        *sha1FromHex("52f01ae3aed1e0fa4c7459a648494863e83a548c"),
        *sha1FromHex("8014488a60f4428eef52d01f8c5861ca9565e1ca"),
        *sha1FromHex("1941a29ad2e7b7bba8a29e6440c95ef5cf76b01d"),
        *sha1FromHex("418099824af6bf1cd7f8bb44f61e3a9cc3007dae"),
        *sha1FromHex("b036864cf0016be0581449ef29fb52b2e58d78a4"),
        *sha1FromHex("8c854b087417a92425110cc71e23c944d6997806"),
        *sha1FromHex("4ec538065fdc8acf49674300cbdec5b80cc05a0d"),
        *sha1FromHex("f9a62bef04cc8f499633e4023acc7675a92771f0"),
        *sha1FromHex("bc038e163d21d9a1181b60cf90b4d03efad9e0c5"),
        *sha1FromHex("077111b996e5c4f47d20ec29c2938504b53a8e76"),
        *sha1FromHex("071d697c4ddb66d3b210f36c7bf878502e79845b"),
        *sha1FromHex("49514e8f74fea50e77273c0297086d67e58123e8"),
        *sha1FromHex("ba9bcfb3731a6cb17dba219a8d37ea4d52332256"),
        *sha1FromHex("15b303b288f4707e5d0af28367c8ce51cdeab490"),
        *sha1FromHex("692d566b05434d8c66a55bdff486698e0fc96095"),
        *sha1FromHex("a4bea41be83d816f793f3fad97d268f71ad99bf9"),
        *sha1FromHex("e8ed31605cc7d6660691998f024eed6ba8b4a33f"),
        *sha1FromHex("456d8800ee7b1b1e2d2dfe4d4654a9c3bb9bd30f"),
        *sha1FromHex("9047f6e9b77c6a44accb46c2237609b80e459fdc"),
        *sha1FromHex("e04eec80bbe0aaf8177e0c707c05ed541fb50a23"),
        *sha1FromHex("701da16b7595050ac1525579aaba40783e49cf8b"),
        *sha1FromHex("c49d2262b0cc65b649fe8add2daf91a3cf73cc45"),
        *sha1FromHex("26776d2bd97bae6d6a8e5fa9c1fd59ae7ca5dee5"),
        *sha1FromHex("b8312bacb25ec0f08560fd95e9edd08e821ce6cd"),
        *sha1FromHex("7913710afcda69b359b5f3d614ddae8614403ef2"),
        *sha1FromHex("acb69b362358cdfe8a7394196696c4296acbc3a0"),
        *sha1FromHex("177710cd82c3019903392a77affa4615145402a3"),
};

const char *SaveManager::s_courseAbbreviations[0x20] = {
        "MC",
        "MMM",
        "MG",
        "GV",
        "TF",
        "CM",
        "DKSC",
        "WGM",
        "LC",
        "DC",
        "MH",
        "MT",
        "BC",
        "RR",
        "DDR",
        "KC",
        "rPB",
        "rMC",
        "rWS",
        "rDKM",
        "rYF",
        "rDH",
        "rPG",
        "rDS",
        "rMC3",
        "rGV2",
        "rMR",
        "rSL",
        "rBC",
        "rDKJP",
        "rBC3",
        "rSGB",
};

} // namespace System

extern "C" {
void SaveManager_EraseLicense(u32 licenseId) {
    return System::SaveManager::Instance()->eraseLicense(licenseId);
}

void SaveManager_EraseSPLicense(void) {
    System::SaveManager::Instance()->eraseSPLicense();
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

u32 SaveManager_GetTAGhostTagVisibility(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TAGhostTagVisibility>();
    return static_cast<u32>(value);
}
}
