#include "TrackPackManager.hh"

#include "sp/settings/IniReader.hh"
#include "sp/storage/Storage.hh"
#include "sp/vanillaTracks/vs.hh"

#include <game/system/RaceConfig.hh>
#include <game/util/Registry.hh>

#include <charconv>
#include <cstring>

#define TRACK_PACK_DIRECTORY L"Track Packs"
#define TRACK_DB L"WiimmDB.ini"

namespace SP {

u32 slotToCourse(u32 slotId) {
    // clang-format off
    switch (slotId) {
    case 11: return 0x8;
    case 12: return 0x1;
    case 13: return 0x2;
    case 14: return 0x4;
    case 21: return 0x0;
    case 22: return 0x5;
    case 23: return 0x6;
    case 24: return 0x7;
    case 31: return 0x9;
    case 32: return 0xF;
    case 33: return 0xB;
    case 34: return 0x3;
    case 41: return 0xE;
    case 42: return 0xA;
    case 43: return 0xC;
    case 44: return 0xD;
    case 51: return 0x10;
    case 52: return 0x14;
    case 53: return 0x19;
    case 54: return 0x1A;
    case 61: return 0x1B;
    case 62: return 0x1F;
    case 63: return 0x17;
    case 64: return 0x12;
    case 71: return 0x15;
    case 72: return 0x1E;
    case 73: return 0x1D;
    case 74: return 0x11;
    case 81: return 0x18;
    case 82: return 0x16;
    case 83: return 0x13;
    case 84: return 0x1C;
    default: panic("Unknown slot id: %d", slotId);
    }
    // clang-format on
}

u32 u32FromSv(std::string_view sv) {
    u32 out = 0;

    auto first = sv.data();
    auto last = sv.data() + sv.size();
    auto [ptr, err] = std::from_chars(first, last, out);

    if (ptr == last) {
        return out;
    } else {
        panic("Failed to parse slotId as integer!");
    }
}

TrackPack::TrackPack(std::string_view manifestView) {
    IniReader iniReader(manifestView);

    char errBuf[256];
    bool prettyNameFound = false;
    bool descriptionFound = false;
    bool authorNamesFound = false;
    while (auto property = iniReader.next()) {
        if (property->section == "Pack Info") {
            if (property->key == "name") {
                m_prettyName = {property->value};
                prettyNameFound = true;
            } else if (property->key == "description") {
                m_description = property->value;
                descriptionFound = true;
            } else if (property->key == "author") {
                m_authorNames = property->value;
                authorNamesFound = true;
            } else {
                u32 maxChars = MIN(property->key.size(), sizeof(errBuf) - 1);
                strncpy(errBuf, property->key.data(), maxChars);
                errBuf[maxChars] = '\0';

                panic("Unknown key name: %s", errBuf);
            }
        } else if (property->section == "Slot Map") {
            auto wiimmId = u32FromSv(property->key);
            auto slotId = u32FromSv(property->value);

            auto courseId = slotToCourse(slotId);
            if (!m_courseMap.push_back({wiimmId, courseId})) {
                panic("Too many courses in track pack!");
            };
        } else {
            u32 maxChars = MIN(property->section.size(), sizeof(errBuf) - 1);
            strncpy(errBuf, property->section.data(), maxChars);
            errBuf[maxChars] = '\0';

            panic("Unknown section name: %s", errBuf);
        }
    }

    if (!prettyNameFound || !descriptionFound || !authorNamesFound) {
        panic("Missing required key in track pack manifest");
    }
}

u32 TrackPack::getCourseId(u32 wmmId) const {
    for (u16 i = 0; i < m_courseMap.count(); i++) {
        auto [cWmmId, cCourseId] = *m_courseMap[i];
        if (cWmmId == wmmId) {
            return cCourseId;
        }
    }

    panic("Failed to find courseId for wmmId: %d", wmmId);
}

u32 TrackPack::getNthTrack(u32 n) const {
    return (*m_courseMap[n])[0];
}

u16 TrackPack::getTrackCount() const {
    return m_courseMap.count();
}

const wchar_t *TrackPack::getPrettyName() const {
    return m_prettyName.c_str();
}

TrackPackManager::TrackPackManager() {
    loadTrackPacks();
    loadTrackDb();
}

void TrackPackManager::loadTrackPacks() {
    SP_LOG("Loading track packs");

    m_packs.push_back(std::move(TrackPack(vanillaManifest)));

    auto dir = Storage::OpenDir(TRACK_PACK_DIRECTORY);
    if (!dir) {
        SP_LOG("Creating track pack directory");
        Storage::CreateDir(TRACK_PACK_DIRECTORY, true);
        return;
    }

    char manifestBuf[2048];
    while (auto nodeInfo = dir->read()) {
        if (nodeInfo->type != Storage::NodeType::File) {
            continue;
        }

        SP_LOG("Found track pack '%ls'", nodeInfo->name);

        auto len = Storage::FastReadFile(nodeInfo->id, manifestBuf, sizeof(manifestBuf));
        if (!len.has_value() || *len == 0) {
            panic("Failed to read track pack manifest");
        }

        std::string_view manifestView(manifestBuf, *len);
        m_packs.push_back(std::move(TrackPack(manifestView)));
    }
}

void TrackPackManager::loadTrackDb() {
    SP_LOG("Loading track DB");

    // Load up the wiimm db, which is pretty large, so we cannot
    // just put it on the stack, so we get the size from stat and
    // allocate a buffer that fits this exact size.
    auto nodeInfo = Storage::Stat(TRACK_DB);
    if (!nodeInfo.has_value()) {
        SP_LOG("No track DB found!");
        return;
    }

    std::string trackDbBuf;
    trackDbBuf.resize(nodeInfo->size);

    auto len = Storage::FastReadFile(nodeInfo->id, trackDbBuf.data(), nodeInfo->size);
    if (!len.has_value() || *len == 0) {
        panic("Failed to read track DB!");
    }

    trackDbBuf.resize(*len);

    IniReader trackDbIni(trackDbBuf);
    while (auto property = trackDbIni.next()) {
        if (property->key == "trackname") {
            u32 wiimmId = u32FromSv(property->section);
            auto kv = std::make_tuple<u32, WFixedString<64>>(std::move(wiimmId),
                    std::move(WFixedString<64>(property->value)));

            m_trackDb.push_back(kv);
        }
    }
}

size_t TrackPackManager::getPackCount() const {
    return m_packs.size();
}

const TrackPack &TrackPackManager::getSelectedTrackPack() const {
    auto &trackPackInfo = System::RaceConfig::Instance()->m_packInfo;
    return m_packs[trackPackInfo.m_selectedTrackPack];
}

const wchar_t *TrackPackManager::getTrackName(u32 courseId) const {
    for (auto &[wiimmId, name] : m_trackDb) {
        if (wiimmId == courseId) {
            return name.c_str();
        }
    }

    SP_LOG("Failed to find track name for wiimmId: %d", courseId);
    return L"Unknown Track";
}

const TrackPack &TrackPackManager::getNthPack(u32 n) const {
    return m_packs[n];
}

TrackPackManager &TrackPackManager::Instance() {
    if (s_instance == nullptr) {
        panic("TrackPackManager not initialized");
    }

    return *s_instance;
}

void TrackPackManager::CreateInstance() {
    if (s_instance == nullptr) {
        s_instance = new TrackPackManager;
    }
}

void TrackPackManager::DestroyInstance() {
    delete s_instance;
    s_instance = nullptr;
}

void TrackPackInfo::getTrackPath(char *out, u32 outSize, bool splitScreen) const {
    SP_LOG("Getting track path for 0x%02x", m_selectedWiimmId);

    if (isVanilla()) {
        auto courseFileName = Registry::courseFilenames[m_selectedCourseId];

        if (splitScreen) {
            snprintf(out, outSize, "Race/Course/%s_d", courseFileName);
        } else {
            snprintf(out, outSize, "Race/Course/%s", courseFileName);
        }

        SP_LOG("Vanilla Track path: %s", out);
    } else {
        snprintf(out, outSize, "/mkw-sp/Tracks/%d", m_selectedWiimmId);
        SP_LOG("Track path: %s", out);
    }
}

bool TrackPackInfo::isVanilla() const {
    return m_selectedTrackPack == 0;
}

u32 TrackPackInfo::getSelectedCourse() const {
    return m_selectedCourseId;
}

void TrackPackInfo::selectCourse(u32 wiimmId) {
    auto &trackPackManager = TrackPackManager::Instance();

    m_selectedWiimmId = wiimmId;
    m_selectedCourseId = trackPackManager.getSelectedTrackPack().getCourseId(wiimmId);
}

TrackPackManager *TrackPackManager::s_instance = nullptr;

} // namespace SP
