#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/FixedString.hh"
#include "sp/ShaUtil.hh"
#include "sp/storage/Storage.hh"

#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace UI {
class LayoutUIControl;
}

namespace SP {

enum class TrackGameMode {
    None = 0,

    Race = 1 << 0,
    Balloon = 1 << 1,
    Coin = 1 << 2,
};

class Track {
public:
    Track(Sha1 sha1) : sha1(sha1){};

    void parse(std::string_view key, std::string_view value);
    u32 getCourseId() const;

    WFixedString<64> name = {};
    bool isArena = false;
    u32 slotId = 0;
    Sha1 sha1;

private:
    u32 getRaceCourseId() const;
    u32 getBattleCourseId() const;
};

class TrackPack {
public:
    TrackPack(std::string_view manifest);

    TrackGameMode getSupportedModes() const;
    u16 getTrackCount(TrackGameMode mode) const;
    std::optional<Sha1> getNthTrack(u32 n, TrackGameMode mode) const;

    const char *getParseError() const;
    const wchar_t *getPrettyName() const;
    const Track *getUnreleasedTrack(Sha1 id) const;

private:
    const std::vector<Sha1> &getTrackList(TrackGameMode mode) const;

    const char *m_parseError;

    std::vector<Sha1> m_raceTracks;
    std::vector<Sha1> m_coinTracks;
    std::vector<Sha1> m_balloonTracks;
    std::vector<Track> m_unreleasedTracks;

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
    const Track &getTrack(Sha1 id) const;

    const TrackPack &getNthPack(u32 n) const;
    const TrackPack &getSelectedTrackPack() const;

    static TrackPackManager &Instance();
    static void CreateInstance();
    static void DestroyInstance();

private:
    std::vector<Track> m_trackDb;
    std::vector<TrackPack> m_packs;

    static TrackPackManager *s_instance;
};

class TrackPackInfo {
public:
    bool isVanilla() const;
    void getTrackPath(char *out, u32 outSize, bool splitScreen) const;

    Sha1 getSelectedSha1() const;
    u32 getSelectedCourse() const;

    void selectCourse(Sha1 id);
    void setTrackMessage(UI::LayoutUIControl *control) const;
    void setTrackMessage(UI::LayoutUIControl *control, const wchar_t *name, u32 courseId) const;

    u32 m_selectedTrackPack = 0;

private:
    // Private as need to be kept in sync
    u32 m_selectedCourseId = 0;
    Sha1 m_selectedSha1 = {};

    // We don't have enough space to store this otherwise.
    static WFixedString<64> s_name;
};

} // namespace SP
