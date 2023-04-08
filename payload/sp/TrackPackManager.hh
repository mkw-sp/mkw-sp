#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/FixedString.hh"
#include "sp/ShaUtil.hh"
#include "sp/storage/Storage.hh"

#include <optional>
#include <span>
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
    Track(Sha1 sha1) : m_sha1(sha1){};

    std::expected<void, const char *> parse(std::string_view key, std::string_view value);
    u32 getCourseId() const;

    std::optional<u32> m_musicId = std::nullopt;
    bool m_isArena = false;
    u32 m_slotId = 0;
    Sha1 m_sha1;
    WFixedString<64> m_name = {};

private:
    u32 getRaceCourseId() const;
    u32 getBattleCourseId() const;
};

class TrackPack {
public:
    static std::expected<TrackPack, const char *> New(std::string_view manifest);

    TrackGameMode getSupportedModes() const;
    u16 getTrackCount(TrackGameMode mode) const;
    std::optional<Sha1> getNthTrack(u32 n, TrackGameMode mode) const;

    const wchar_t *getPrettyName() const;
    const Track *getUnreleasedTrack(Sha1 id) const;

private:
    TrackPack() = default;
    std::expected<void, const char *> parseNew(std::string_view manifest);

    const std::vector<Sha1> &getTrackList(TrackGameMode mode) const;

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

    size_t getPackCount() const;
    const Track &getTrack(Sha1 id) const;

    const TrackPack &getNthPack(u32 n) const;
    const TrackPack &getSelectedTrackPack() const;

    static TrackPackManager &Instance();
    static void CreateInstance();
    static void DestroyInstance();

private:
    std::expected<void, const char *> loadTrackPacks();
    void loadTrackDb();

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
    std::optional<u32> getSelectedMusic() const;

    void selectCourse(Sha1 id);
    void setTrackMessage(UI::LayoutUIControl *control) const;
    void setTrackMessage(UI::LayoutUIControl *control, const wchar_t *name, u32 courseId) const;

    u32 m_selectedTrackPack = 0;

private:
    // Private as need to be kept in sync
    u32 m_selectedCourseId = 0;
    u32 m_selectedMusicId;
    Sha1 m_selectedSha1 = {};
    WFixedString<64> m_name;
};

} // namespace SP
