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

    size_t getPackCount() const;
    const TrackPack &getNthPack(u32 n) const;
    const TrackPack &getSelectedTrackPack() const;
    const wchar_t *getTrackName(u32 courseId) const;

    static TrackPackManager &Instance();
    static void CreateInstance();
    static void DestroyInstance();

private:
    std::vector<std::tuple<u32, WFixedString<64>>> m_trackDb;
    std::vector<TrackPack> m_packs;

    static TrackPackManager *s_instance;
};

class TrackPackInfo {
public:
    bool isVanilla() const;
    void getTrackPath(char *out, u32 outSize, bool splitScreen) const;

    u32 getSelectedCourse() const;
    void selectCourse(u32 wiimmId);

    u32 m_selectedTrackPack = 0;

private:
    // Private as need to be kept in sync
    u32 m_selectedCourseId = 0;
    u32 m_selectedWiimmId = 0;
};

} // namespace SP
