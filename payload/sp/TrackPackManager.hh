#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/FixedString.hh"
#include "sp/storage/Storage.hh"

#include <optional>
#include <string_view>
#include <vector>

namespace SP {

enum class TrackGameMode {
    Race = 1,
    Balloon = 2,
    Coin = 3,
};

class Track {
public:
    bool isValid() {
        return name.m_len != 0 && slotId != 0;
    }

    WFixedString<64> name = {};
    u32 slotId = 0;
};

struct DBEntry {
    u32 wiimmId;
    Track track;
};

class TrackPack {
public:
    TrackPack(std::string_view manifest);

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

    void loadTrackPacks();
    void loadTrackDb();

    size_t getPackCount() const;
    u32 getCourseId(u32 wmmId, TrackGameMode mode) const;

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
    void selectCourse(u32 wiimmId, TrackGameMode mode);

    u32 m_selectedTrackPack = 0;

private:
    // Private as need to be kept in sync
    u32 m_selectedCourseId = 0;
    u32 m_selectedWiimmId = 0;
};

TrackGameMode getTrackGameMode(u32 gameMode, u32 battleType);

} // namespace SP
