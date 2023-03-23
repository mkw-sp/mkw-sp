#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"

#include <vector>

namespace SP {

enum class SupportedGameModes {
    Race = 1 << 0,
    BalloonBattle = 1 << 1,
    CoinRunners = 1 << 2,
};

class TrackPack {
public:
    TrackPack(wchar_t *folderName);

private:
    wchar_t *m_folderName;

    char *m_prettyName;
    char *m_description;
    char *m_authorNames;
    SupportedGameModes *m_supportedModes;
};

class TrackPackManager {
public:
    TrackPackManager();

    bool getTrackPath(wchar_t *out, u32 outSize, const wchar_t *trackName);
private:
    // TODO: Not this!
    CircularBuffer<TrackPack, 0x20> m_packs;
};

} // namespace SP
