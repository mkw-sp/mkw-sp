#include "TrackPackManager.hh"

#include "sp/settings/IniReader.hh"
#include "sp/storage/Storage.hh"

#include <game/util/Registry.hh>

#include <charconv>
#include <cstring>

#include "VanillaTracks.ini"
#define TRACK_PACK_DIRECTORY L"Track Packs"

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
            SP_LOG("Slot map: %d -> %d", wiimmId, slotId);

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

TrackPackManager::TrackPackManager() {
    char manifestBuf[2048];

    m_packs.reset();
    m_selectedTrackPack = 0;

    auto dir = Storage::OpenDir(TRACK_PACK_DIRECTORY);
    if (!dir) {
        SP_LOG("Creating track pack directory");
        Storage::CreateDir(TRACK_PACK_DIRECTORY, true);
        return;
    }

    SP_LOG("Reading track packs");
    m_packs.push_back(std::move(TrackPack(vanillaManifest)));
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

bool TrackPackManager::isVanilla() {
    return m_selectedTrackPack == 0;
}

const TrackPack *TrackPackManager::getSelectedTrackPack() {
    return m_packs[m_selectedTrackPack];
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
    s_instance.emplace();
}

std::optional<TrackPackManager> TrackPackManager::s_instance = std::nullopt;

} // namespace SP
