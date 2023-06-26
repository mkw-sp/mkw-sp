#pragma once

#include "TrackPack.hh"

namespace SP {

class TrackPackManager {
private:
    TrackPackManager();
    TrackPackManager(const TrackPackManager &) = delete;

    void loadTrackPacks();
    void loadTrackMetadata();

public:
    size_t getPackCount() const;
    const Track &getTrack(Sha1 id) const;

    const TrackPack &getNthPack(u32 n) const;
    const TrackPack &getSelectedPack() const;

    static TrackPackManager &Instance();
    static void CreateInstance();
    static void DestroyInstance();

private:
    std::vector<Track> m_trackDb;
    std::vector<TrackPack> m_packs;

    static TrackPackManager *s_instance;
};

} // namespace SP
