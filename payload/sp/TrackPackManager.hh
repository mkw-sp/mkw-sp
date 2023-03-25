#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/storage/Storage.hh"

#include <string>

namespace SP {

enum class SupportedGameModes {
    None = 0,
    Race = 1 << 0,
    BalloonBattle = 1 << 1,
    CoinRunners = 1 << 2,
};

class TrackPack {
public:
    TrackPack(Storage::NodeId manifestNodeId);

private:
    std::string m_prettyName;
    std::string m_description;
    std::string m_authorNames;
    SupportedGameModes m_supportedModes = SupportedGameModes::None;
};

class TrackPackManager {
public:
    TrackPackManager();

    void getTrackPath(char *out, u32 outSize, u32 courseId, bool splitScreen);

    static TrackPackManager *Instance();
    static void CreateInstance();

private:
    // TODO: Not this!
    CircularBuffer<TrackPack, 0x5> m_packs;
    u32 m_selectedTrackPack;
    bool m_hasSelected;

    static std::optional<TrackPackManager> s_instance;
};

} // namespace SP
