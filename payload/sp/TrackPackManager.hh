#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/FixedString.hh"
#include "sp/storage/Storage.hh"

#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace SP {

enum class TrackGameMode {
    None = 0,

    Race = 1 << 0,
    Balloon = 1 << 1,
    Coin = 1 << 2,
};

class Track {
public:
    u32 getCourseId() const;

    std::array<u8, 0x14> sha1 = {};
    WFixedString<64> name = {};
    bool isArena = false;
    u32 slotId = 0;

private:
    u32 getRaceCourseId() const;
    u32 getBattleCourseId() const;
};

struct DBEntry {
    u32 wiimmId;
    Track track;
};

class TrackPack {
public:
    TrackPack(std::string_view manifest);

    TrackGameMode getSupportedModes() const;
    u16 getTrackCount(TrackGameMode mode) const;
    u32 getNthTrack(u32 n, TrackGameMode mode) const;

    const wchar_t *getPrettyName() const;

private:
    const std::vector<u32> &getTrackList(TrackGameMode mode) const;

    std::vector<u32> m_raceTracks;
    std::vector<u32> m_coinTracks;
    std::vector<u32> m_balloonTracks;

    FixedString<64> m_authorNames;
    FixedString<128> m_description;
    WFixedString<64> m_prettyName;
};

class TrackPackManager {
public:
    TrackPackManager();
    TrackPackManager(const TrackPackManager &) = delete;

    void loadTrackPacks();
    void loadTrackDb();

    size_t getPackCount() const;
    const Track &getTrack(u32 wmmId) const;
    std::optional<u32> wiimmIdFromSha1(std::span<const u8, 0x14> sha1) const;

    const TrackPack &getNthPack(u32 n) const;
    const TrackPack &getSelectedTrackPack() const;
    const wchar_t *getTrackName(u32 wiimmId) const;

    static TrackPackManager &Instance();
    static void CreateInstance();
    static void DestroyInstance();

private:
    std::vector<DBEntry> m_trackDb;
    std::vector<TrackPack> m_packs;

    static TrackPackManager *s_instance;
};

class TrackPackInfo {
public:
    bool isVanilla() const;
    void getTrackPath(char *out, u32 outSize, bool splitScreen) const;

    u32 getSelectedCourse() const;
    u32 getSelectedWiimmId() const;
    std::span<const u8, 0x14> getSelectedSha1() const;
    void selectCourse(u32 wiimmId);

    u32 m_selectedTrackPack = 0;

private:
    // Private as need to be kept in sync
    std::array<u8, 0x14> m_selectedSha1 = {};
    u32 m_selectedCourseId = 0;
    u32 m_selectedWiimmId = 0;
};

} // namespace SP
