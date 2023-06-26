#include "TrackPackManager.hh"

#include "sp/storage/Storage.hh"

#include <game/system/ResourceManager.hh>
#include <game/ui/SectionManager.hh>
#include <protobuf/TrackPacks.pb.h>
#include <vendor/nanopb/pb_decode.h>

#include <cstdio>

namespace SP {

#define TRACK_PACK_DIRECTORY L"Track Packs"
#define TRACK_DIRECTORY L"Tracks"

std::span<u8> readVanillaTrack(std::array<char, 0x28 + 1> sha1Hex) {
    auto *resourceManager = System::ResourceManager::Instance();

    char pathBuf[sizeof("vanillaTracks/") + sizeof(sha1Hex) + sizeof(".pb.bin")];
    snprintf(pathBuf, std::size(pathBuf), "vanillaTracks/%s.pb.bin", sha1Hex.data());

    size_t size = 0;
    void *manifest = resourceManager->getFile(System::ResourceType::Menu, pathBuf, &size);
    if (size == 0) {
        panic("Failed to load vanilla track metadata for %s", sha1Hex.data());
    }

    return std::span(reinterpret_cast<u8 *>(manifest), size);
}

std::span<u8> readSDTrack(std::vector<u8> &manifestBuf, std::array<char, 0x28 + 1> sha1Hex) {
    wchar_t pathBuf[sizeof("Tracks/") + sizeof(sha1Hex) + sizeof(".pb.bin")];
    swprintf(pathBuf, std::size(pathBuf), L"Tracks/%s.pb.bin", sha1Hex.data());

    auto trackHandle = Storage::Open(pathBuf, "r");
    if (!trackHandle.has_value()) {
        panic("Could not find track metadata for %s", sha1Hex.data());
    }

    manifestBuf.resize(trackHandle->size());
    if (!trackHandle->read(manifestBuf.data(), manifestBuf.size(), 0)) {
        panic("Could not read track metadata for %s", sha1Hex.data());
    }

    return std::span(manifestBuf.data(), manifestBuf.size());
}

TrackPackManager::TrackPackManager() {
    loadTrackPacks();
    loadTrackMetadata();
}

void TrackPackManager::loadTrackPacks() {
    SP_LOG("Loading track packs");

    auto *resourceManager = System::ResourceManager::Instance();

    size_t size = 0;
    auto *vanillaManifestRaw = reinterpret_cast<const u8 *>(
            resourceManager->getFile(System::ResourceType::Menu, "vanillaTracks.pb.bin", &size));
    assert(size != 0);

    std::span vanillaManifest(vanillaManifestRaw, size);
    m_packs.push_back(std::move(TrackPack::New(vanillaManifest).value()));

    auto dir = Storage::OpenDir(TRACK_PACK_DIRECTORY);
    if (!dir) {
        SP_LOG("Creating track pack directory");
        Storage::CreateDir(TRACK_PACK_DIRECTORY, true);
        return;
    }

    std::vector<u8> manifestBuf;
    while (auto nodeInfo = dir->read()) {
        if (nodeInfo->type != Storage::NodeType::File) {
            continue;
        }

        SP_LOG("Found track pack '%ls'", nodeInfo->name);
        manifestBuf.resize(nodeInfo->size);

        auto len = Storage::FastReadFile(nodeInfo->id, manifestBuf.data(), nodeInfo->size);
        if (!len.has_value() || *len == 0) {
            SP_LOG("Failed to read track pack manifest!");
            continue;
        }

        manifestBuf.resize(*len);

        auto res = TrackPack::New(manifestBuf);
        if (!res.has_value()) {
            SP_LOG("Failed to read track pack manifest: %s", res.error());
            continue;
        }

        m_packs.push_back(std::move(*res));
    }
}

void TrackPackManager::loadTrackMetadata() {
    SP_LOG("Loading track metadata");

    auto dir = Storage::OpenDir(TRACK_DIRECTORY);
    assert(dir.has_value()); // Was created in loadTrackPacks

    bool foundVanilla = false;
    std::span<u8> manifestView;
    std::vector<u8> manifestBuf;
    for (auto &pack : m_packs) {
        SP_LOG("Loading track metadata for pack: %ls", pack.getPrettyName());
        for (auto mode : s_trackModes) {
            size_t trackCount = pack.getTrackCount(mode);
            m_trackDb.reserve(m_trackDb.size() + trackCount);

            std::optional<Sha1> trackSha;
            for (u16 i = 0; (trackSha = pack.getNthTrack(i, mode)); i += 1) {
                auto trackShaHex = sha1ToHex(trackSha.value());
                if (foundVanilla) {
                    manifestView = readSDTrack(manifestBuf, trackShaHex);
                } else {
                    manifestView = readVanillaTrack(trackShaHex);
                }

                auto track = Track::FromFile(manifestView, *trackSha);
                m_trackDb.push_back(std::move(track));
            }
        }

        foundVanilla = true;
    }
}

size_t TrackPackManager::getPackCount() const {
    return m_packs.size();
}

const Track &TrackPackManager::getTrack(Sha1 sha1) const {
    for (auto &track : m_trackDb) {
        if (track.m_sha1 == sha1) {
            return track;
        }
    }

    auto hex = sha1ToHex(sha1);
    panic("Unknown sha1 id: %s", hex.data());
}

const TrackPack &TrackPackManager::getNthPack(u32 n) const {
    return m_packs[n];
}

const TrackPack &TrackPackManager::getSelectedPack() const {
    auto *globalContext = UI::SectionManager::Instance()->globalContext();
    return m_packs[globalContext->m_currentPack];
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

TrackPackManager *TrackPackManager::s_instance = nullptr;

} // namespace SP
