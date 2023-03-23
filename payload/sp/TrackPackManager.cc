#include "TrackPackManager.hh"

#include "sp/storage/Storage.hh"

#define TRACK_PACK_DIRECTORY L"Track Packs"

namespace SP {

TrackPack::TrackPack(wchar_t *folderName) {
    m_folderName = folderName;
}

TrackPackManager::TrackPackManager() {
    m_packs.reset();

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
            m_packs.push_back(std::move(TrackPack(nodeInfo->name)));
        }
    }
}

bool TrackPackManager::getTrackPath(wchar_t *out, u32 outSize, const wchar_t *trackName, size_t len) {
    // TODO: Implement properly
    swprintf(out, outSize, L"%ls/%ls", TRACK_PACK_DIRECTORY, trackName);
    return true
}

} // namespace SP
