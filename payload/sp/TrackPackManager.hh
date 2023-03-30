#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/FixedString.hh"
#include "sp/storage/Storage.hh"

#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

namespace SP {

#define MAX_SLOT_COUNT 256
#define MAX_TRACKPACK_COUNT 0x5

enum class SupportedGameModes {
    None = 0,
    Race = 1 << 0,
    BalloonBattle = 1 << 1,
    CoinRunners = 1 << 2,
};

class TrackPack {
public:
    TrackPack(std::string_view manifest);

    u16 getTrackCount() const;
    u32 getNthTrack(u32 n) const;
    u32 getCourseId(u32 wmmId) const;

    const wchar_t *getPrettyName() const;

private:
    CircularBuffer<std::array<u32, 2>, MAX_SLOT_COUNT> m_courseMap;

    FixedString<64> m_authorNames;
    FixedString<128> m_description;
    WFixedString<64> m_prettyName;
    SupportedGameModes m_supportedModes = SupportedGameModes::None;
};

class TrackPackManager {
public:
    TrackPackManager();

    void loadTrackPacks();
    void loadTrackDb();

    bool isVanilla();
    size_t getPackCount();
    void getTrackPath(char *out, u32 outSize, u32 wmmId, bool splitScreen);

    const TrackPack *getSelectedTrackPack();
    const wchar_t *getTrackName(u32 courseId);
    const TrackPack *getNthPack(u32 n);

    static TrackPackManager *Instance();
    static void CreateInstance();

    // This must be called before Scene swaps.
    void unloadTrackDb();

    u32 m_selectedTrackPack;

private:
    std::optional<std::vector<std::tuple<u32, WFixedString<64>>>> m_trackDb;
    CircularBuffer<TrackPack, MAX_TRACKPACK_COUNT> m_packs;

    static std::optional<TrackPackManager> s_instance;
};

} // namespace SP
