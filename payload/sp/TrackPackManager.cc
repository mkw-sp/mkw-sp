#include "TrackPackManager.hh"

#include "sp/storage/Storage.hh"

#include <game/util/Registry.hh>

#include <cstdio>
#include <cstring>
#include <cwchar>

#define TRACK_PACK_DIRECTORY L"Track Packs"

namespace SP {

TrackPack::TrackPack(Storage::NodeId manifestNodeId) {
    // TODO: Parse manifest
}

TrackPackManager::TrackPackManager() {
    m_packs.reset();
    m_hasSelected = true;
    m_selectedTrackPack = 0;

    auto dir = Storage::OpenDir(TRACK_PACK_DIRECTORY);
    if (!dir) {
        SP_LOG("Creating track pack directory");
        Storage::CreateDir(TRACK_PACK_DIRECTORY, true);
        return;
    }

    SP_LOG("Reading track packs");
    while (auto nodeInfo = dir->read()) {
        if (nodeInfo->type == Storage::NodeType::Dir) {
            SP_LOG("Found track pack '%ls'", nodeInfo->name);
            if (!m_packs.push_back(std::move(TrackPack(nodeInfo->id)))) {
                SP_LOG("Reached max track packs!");
                break;
            }
        }
    }
}

void TrackPackManager::getTrackPath(char *out, u32 outSize, u32 courseId, bool splitScreen) {
    SP_LOG("Getting track path for 0x%02x", courseId);

    if (!m_hasSelected) {
        SP_LOG("No track pack selected");

        auto courseFileName = Registry::courseFilenames[courseId];
        if (splitScreen) {
            snprintf(out, outSize, "Race/Course/%s_d", courseFileName);
        } else {
            snprintf(out, outSize, "Race/Course/%s", courseFileName);
        }

        SP_LOG("Vanilla Track path: %s", out);
        return;
    } else {
        // Do we want courseId to be vanilla course IDs,
        // then map from vanilla to custom course IDs in manifest,
        // or do we want to use custom course IDs and break everything?
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
