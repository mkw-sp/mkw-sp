#pragma once

#include <Common.hh>

#include "sp/CircularBuffer.hh"
#include "sp/FixedString.hh"
#include "sp/ShaUtil.hh"
#include "sp/storage/Storage.hh"

#include <limits>
#include <optional>
#include <span>
#include <vector>

namespace UI {
class LayoutUIControl;
}

namespace SP {

enum class TrackGameMode {
    Race = 1 << 0,
    Balloon = 1 << 1,
    Coin = 1 << 2,
};

class Track {
public:
    Track(Sha1 sha1) : m_sha1(sha1) {}

    std::expected<void, const char *> parse(std::string_view key, std::string_view value);
    u32 getCourseId() const;

    u16 m_musicId = std::numeric_limits<u16>::max();
    bool m_isArena = false;
    u8 m_slotId = 0;
    Sha1 m_sha1;
    WFixedString<48> m_name = {};

private:
    u32 getRaceCourseId() const;
    u32 getBattleCourseId() const;
};

static_assert(sizeof(Track) == 0x7c);

class TrackPack {
public:
    static std::expected<TrackPack, const char *> New(std::string_view manifest);

    bool contains(const Sha1 &id) const;
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
    std::optional<Sha1> getNormalisedSha1(Sha1 aliasedSha1) const;

    const TrackPack &getNthPack(u32 n) const;
    const TrackPack &getSelectedTrackPack() const;

    static TrackPackManager &Instance();
    static void CreateInstance();
    static void DestroyInstance();

private:
    std::expected<void, const char *> loadTrackPacks();
    void loadTrackDb();
    bool anyPackContains(const Sha1 &sha1);
    void parseAlias(Sha1 aliasedSha1, Sha1 sha1);

    std::vector<Track> m_trackDb;
    std::vector<TrackPack> m_packs;
    std::vector<std::pair<Sha1, Sha1>> m_aliases;

    static TrackPackManager *s_instance;
};

class TrackPackInfo {
public:
    void getTrackPath(char *out, u32 outSize, bool splitScreen) const;

    // May be different to the sha1 passed to selectCourse, due to
    // My Stuff replacements requiring matching Ghost hashes.
    Sha1 getCourseSha1() const;

    u32 getSelectedCourse() const;
    const wchar_t *getCourseName() const;
    std::optional<u32> getSelectedMusic() const;

    void selectCourse(Sha1 id);

private:
    // Private as need to be kept in sync
    u32 m_selectedCourseId = 0;
    u32 m_selectedMusicId;
    Sha1 m_selectedSha1 = {};
    WFixedString<48> m_name;
};

} // namespace SP
