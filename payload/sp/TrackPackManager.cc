#include "TrackPackManager.hh"

#include "sp/settings/IniReader.hh"
#include "sp/storage/Storage.hh"
#include "sp/vanillaTracks/vs.hh"

#include <game/util/Registry.hh>

#include <algorithm>
#include <charconv>
#include <codecvt>
#include <cstring>
#include <locale>

#define TRACK_PACK_DIRECTORY L"Track Packs"
#define TRACK_DB L"WiimmDB.ini"

namespace SP {

u32 courseToSlot(u32 courseId) {
    switch (courseId) {
    case 11: return 0x8;
    case 12: return 0x1;
    case 13: return 0x2;
    case 14: return 0x3;
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
    case 84: return 0x17;
    default: panic("Unknown course id: %d", courseId);
    }
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
                m_prettyName = property->value;
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

            auto courseId = courseToSlot(slotId);
            m_courseMap.push_back({wiimmId, courseId});
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

TrackPackManager::TrackPackManager() {
    m_selectedTrackPack = 1;

    loadTrackPacks();
    loadTrackDb();
}

void TrackPackManager::loadTrackPacks() {
    SP_LOG("Loading track packs");

    m_packs.reset();
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
        if (!m_packs.push_back(std::move(TrackPack(manifestView)))) {
            SP_LOG("Reached max track packs!");
            break;
        }
    }
}

void TrackPackManager::loadTrackDb() {
    SP_LOG("Loading track DB");

    m_trackDb.emplace();

    // Load up the wiimm db, which is pretty large, so we cannot
    // just put it on the stack, so we get the size from stat and
    // allocate a buffer that fits this exact size.
    auto nodeInfo = Storage::Stat(TRACK_DB);
    if (!nodeInfo.has_value()) {
        SP_LOG("No track DB found!");
        return;
    }

    std::string trackDbBuf("");
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
            auto valueOwned = std::string(property->value);

            std::wstring name;

            name.resize(property->value.size() + 1);
            auto written = swprintf(name.data(), name.size(), L"%s", valueOwned.c_str());
            name.resize(written);

            auto kv = std::make_tuple<u32, std::wstring>(std::move(wiimmId), std::move(name));
            m_trackDb->push_back(kv);
        }
    }
}

bool TrackPackManager::isVanilla() {
    return m_selectedTrackPack == 0;
}

const TrackPack *TrackPackManager::getSelectedTrackPack() {
    return m_packs[m_selectedTrackPack];
}

const wchar_t *TrackPackManager::getTrackName(u32 courseId) {
    for (auto &[wiimmId, name] : *m_trackDb) {
        if (wiimmId == courseId) {
            return name.c_str();
        }
    }

    SP_LOG("Failed to find track name for wiimmId: %d", courseId);
    return L"Unknown Track";
}

void TrackPackManager::getTrackPath(char *out, u32 outSize, u32 wiimmId, bool splitScreen) {
    SP_LOG("Getting track path for 0x%02x", wiimmId);

    if (isVanilla()) {
        auto courseId = getSelectedTrackPack()->getCourseId(wiimmId);
        auto courseFileName = Registry::courseFilenames[courseId];

        if (splitScreen) {
            snprintf(out, outSize, "Race/Course/%s_d", courseFileName);
        } else {
            snprintf(out, outSize, "Race/Course/%s", courseFileName);
        }

        SP_LOG("Vanilla Track path: %s", out);
    } else {
        snprintf(out, outSize, "/mkw-sp/Tracks/%d", wiimmId);
        SP_LOG("Track path: %s", out);
    }
}

TrackPackManager *TrackPackManager::Instance() {
    return s_instance.operator->();
}

void TrackPackManager::CreateInstance() {
    if (!s_instance.has_value()) {
        s_instance = TrackPackManager();
    } else if (!s_instance->m_trackDb.has_value()) {
        // Reload the track packs and track db
        // if a scene change has occurred.
        s_instance->loadTrackPacks();
        s_instance->loadTrackDb();
    }
}

void TrackPackManager::unloadTrackDb() {
    m_trackDb.reset();
}

std::optional<TrackPackManager> TrackPackManager::s_instance = std::nullopt;

} // namespace SP
