#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/storage/Storage.hh"

#include <array>
#include <string>
#include <string_view>

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
    u32 getSlotId(u32 wmmId) const;

private:
    CircularBuffer<std::array<u32, 2>, MAX_SLOT_COUNT> m_slotMap;

    std::string m_prettyName;
    std::string m_description;
    std::string m_authorNames;
    SupportedGameModes m_supportedModes = SupportedGameModes::None;
};

class TrackPackManager {
public:
    TrackPackManager();

    bool isVanilla();
    const TrackPack *getSelectedTrackPack();
    void getTrackPath(char *out, u32 outSize, u32 wmmId, bool splitScreen);

    static TrackPackManager *Instance();
    static void CreateInstance();

    // Currently an ID from the wiimm DB, uses the Pack
    // to get the slotID to use for the Scenario courseId
    u32 m_selectedTrack;

private:
    // TODO: Not this!
    CircularBuffer<TrackPack, MAX_TRACKPACK_COUNT> m_packs;
    u32 m_selectedTrackPack;

    static std::optional<TrackPackManager> s_instance;
};

} // namespace SP
