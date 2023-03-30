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

u32 u32FromSv(std::string_view sv) {
    u32 out = 0;

    auto first = sv.data();
    auto last = sv.data() + sv.size();
    auto [ptr, err] = std::from_chars(first, last, out);

    if (ptr == last) {
        return out;
    } else {
        panic("Failed to parse slotId to wmmId!");
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

            m_slotMap.push_back({wiimmId, slotId});
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

u32 TrackPack::getSlotId(u32 wmmId) const {
    for (u16 i = 0; i < m_slotMap.count(); i++) {
        auto [cWmmId, cSlotId] = *m_slotMap[i];
        if (cWmmId == wmmId) {
            return cSlotId;
        }
    }

    panic("Failed to find slotId for wmmId: %d", wmmId);
}

u32 TrackPack::getNthTrack(u32 n) const {
    return (*m_slotMap[n])[0];
}

u16 TrackPack::getTrackCount() const {
    return m_slotMap.count();
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
    m_trackDb.emplace();

    // Load up the wiimm db, which is pretty large, so we cannot
    // just put it on the stack, so we get the size from stat and
    // allocate a buffer that fits this exact size.
    auto nodeInfo = Storage::Stat(TRACK_DB);
    if (!nodeInfo.has_value()) {
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
        if (property->key == "name") {
            u32 wiimmId = u32FromSv(property->section);
            auto nullTermName = std::string(property->value).c_str();

            SP_LOG("Found track: %d", wiimmId);

            std::wstring name;

            name.resize(property->value.size() + 1);
            auto written = swprintf(name.data(), name.size(), L"%s", nullTermName);
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

    SP_LOG("Failed to find track name for courseId: %d", courseId);
    return L"Unknown Track";
}

void TrackPackManager::getTrackPath(char *out, u32 outSize, u32 courseId, bool splitScreen) {
    SP_LOG("Getting track path for 0x%02x", courseId);

    if (isVanilla()) {
        auto slotId = getSelectedTrackPack()->getSlotId(courseId);
        auto courseFileName = Registry::courseFilenames[slotId];

        if (splitScreen) {
            snprintf(out, outSize, "Race/Course/%s_d", courseFileName);
        } else {
            snprintf(out, outSize, "Race/Course/%s", courseFileName);
        }

        SP_LOG("Vanilla Track path: %s", out);
    } else {
        snprintf(out, outSize, "/mkw-sp/Tracks/%d", courseId);
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
