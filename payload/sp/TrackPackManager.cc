#include "TrackPackManager.hh"

#include "sp/ThumbnailManager.hh"
#include "sp/settings/IniReader.hh"
#include "sp/storage/Storage.hh"
#include "sp/vanillaTracks.hh"

#include <game/system/RaceConfig.hh>
#include <game/system/SaveManager.hh>
#include <game/ui/UIControl.hh>
#include <game/util/Registry.hh>
#include <vendor/magic_enum/magic_enum.hpp>

#include <charconv>
#include <cstring>

#define TRACK_PACK_DIRECTORY L"Track Packs"
#define TRACK_DB L"TrackDB.ini"

using namespace magic_enum::bitwise_operators;

namespace SP {

// clang-format off
u32 Track::getRaceCourseId() const {
    switch (m_slotId) {
    case 11: return 0x8;
    case 12: return 0x1;
    case 13: return 0x2;
    case 14: return 0x4;
    case 21: return 0x0;
    case 22: return 0x5;
    case 23: return 0x6;
    case 24: return 0x7;
    case 31: return 0x9;
    case 32: return 0xF;
    case 33: return 0xB;
    case 34: return 0x3;
    case 41: return 0xE;
    case 42: return 0xA;
    case 43: return 0xC;
    case 44: return 0xD;
    case 51: return 0x10;
    case 52: return 0x14;
    case 53: return 0x19;
    case 54: return 0x1A;
    case 61: return 0x1B;
    case 62: return 0x1F;
    case 63: return 0x17;
    case 64: return 0x12;
    case 71: return 0x15;
    case 72: return 0x1E;
    case 73: return 0x1D;
    case 74: return 0x11;
    case 81: return 0x18;
    case 82: return 0x16;
    case 83: return 0x13;
    case 84: return 0x1C;
    default: panic("Unknown race slot ID: %d", m_slotId);
    }
}

u32 Track::getBattleCourseId() const {
    switch (m_slotId) {
    case 11: return 0x21;
    case 12: return 0x20;
    case 13: return 0x23;
    case 14: return 0x22;
    case 15: return 0x24;
    case 21: return 0x27;
    case 22: return 0x28;
    case 23: return 0x29;
    case 24: return 0x25;
    case 25: return 0x26;
    default: panic("Unknown battle slot ID: %d", m_slotId);
    }
}
// clang-format on

std::expected<u32, const char *> u32FromSv(std::string_view sv) {
    u32 out = 0;

    auto first = sv.data();
    auto last = sv.data() + sv.size();
    auto [ptr, err] = std::from_chars(first, last, out);

    if (ptr == last) {
        return out;
    } else {
        return std::unexpected("Failed to parse integer!");
    }
}

u32 Track::getCourseId() const {
    if (m_isArena) {
        return getBattleCourseId();
    } else {
        return getRaceCourseId();
    }
}

std::expected<void, const char *> Track::parse(std::string_view key, std::string_view value) {
    if (key == "trackname") {
        m_name.setUTF8(value);
    } else if (key == "slot") {
        m_slotId = TRY(u32FromSv(value));
    } else if (key == "mslot") {
        m_musicId = TRY(u32FromSv(value));
    } else if (key == "type") {
        if (value == "1") {
            m_isArena = false;
        } else if (value == "2") {
            m_isArena = true;
        }
    }

    return {};
}

std::expected<std::vector<Sha1>, const char *> parseTracks(std::string_view tracks) {
    std::vector<Sha1> parsedTrackIds;
    size_t startPos = 0;
    size_t startOffset = 0;

    for (char c : tracks) {
        if (c == ',' || (startPos + startOffset) == tracks.size()) {
            auto track = tracks.substr(startPos, startOffset);
            startPos = startPos + startOffset + 1;
            startOffset = 0;

            parsedTrackIds.push_back(TRY(sha1FromHex(track)));
        } else if (c != ' ') {
            startOffset++;
        }
    }

    if (startOffset != 0) {
        auto track = tracks.substr(startPos, startOffset);
        parsedTrackIds.push_back(TRY(sha1FromHex(track)));
    }

    return parsedTrackIds;
}

std::expected<TrackPack, const char *> TrackPack::New(std::string_view manifestView) {
    TrackPack self;
    self.parseNew(manifestView);
    return self;
}

std::expected<void, const char *> TrackPack::parseNew(std::string_view manifestView) {
    IniReader iniReader(manifestView);

    bool prettyNameFound = false;
    bool descriptionFound = false;
    bool authorNamesFound = false;
    while (auto property = iniReader.next()) {
        auto [section, key, value] = *property;

        if (section == "Pack Info") {
            if (key == "name") {
                m_prettyName.setUTF8(value);
                prettyNameFound = true;
            } else if (key == "description") {
                m_description = value;
                descriptionFound = true;
            } else if (key == "author") {
                m_authorNames = value;
                authorNamesFound = true;
            } else if (key == "race") {
                m_raceTracks = TRY(parseTracks(value));
            } else if (key == "balloon") {
                m_balloonTracks = TRY(parseTracks(value));
            } else if (key == "coin") {
                m_coinTracks = TRY(parseTracks(value));
            } else {
                return std::unexpected("Unknown key in track pack manifest");
            }
        } else {
            auto sha1 = TRY(sha1FromHex(section));
            if (m_unreleasedTracks.empty() || m_unreleasedTracks.back().m_sha1 != sha1) {
                m_unreleasedTracks.emplace_back(sha1);
            }

            auto &track = m_unreleasedTracks.back();
            track.parse(key, value);
        }
    }

    if (!prettyNameFound || !descriptionFound || !authorNamesFound) {
        return std::unexpected("Missing required key in track pack manifest");
    } else if (m_raceTracks.empty() && m_balloonTracks.empty() && m_coinTracks.empty()) {
        return std::unexpected("No tracks found in track pack manifest");
    }

    return {};
}

const Track *TrackPack::getUnreleasedTrack(Sha1 sha1) const {
    for (const auto &track : m_unreleasedTracks) {
        if (track.m_sha1 == sha1) {
            return &track;
        }
    }

    return nullptr;
}

const std::vector<Sha1> &TrackPack::getTrackList(TrackGameMode mode) const {
    if (mode == TrackGameMode::Race) {
        return m_raceTracks;
    } else if (mode == TrackGameMode::Balloon) {
        return m_balloonTracks;
    } else if (mode == TrackGameMode::Coin) {
        return m_coinTracks;
    } else {
        panic("Unknown track game mode: %d", static_cast<u32>(mode));
    }
}

constexpr TrackGameMode modes[] = {
        TrackGameMode::Race,
        TrackGameMode::Coin,
        TrackGameMode::Balloon,
};

TrackGameMode TrackPack::getSupportedModes() const {
    auto supportedModes = static_cast<TrackGameMode>(0);
    for (auto mode : modes) {
        if (!getTrackList(mode).empty()) {
            supportedModes |= mode;
        }
    }

    return supportedModes;
}

bool TrackPack::contains(Sha1 sha1) const {
    for (auto mode : modes) {
        for (auto track : getTrackList(mode)) {
            if (track == sha1) {
                return true;
            }
        }
    }

    return false;
}

u16 TrackPack::getTrackCount(TrackGameMode mode) const {
    return getTrackList(mode).size();
}

std::optional<Sha1> TrackPack::getNthTrack(u32 n, TrackGameMode mode) const {
    auto &trackList = getTrackList(mode);
    if (trackList.size() <= n) {
        return std::nullopt;
    } else {
        return trackList[n];
    }
}

const wchar_t *TrackPack::getPrettyName() const {
    return m_prettyName.c_str();
}

TrackPackManager::TrackPackManager() {
    auto res = loadTrackPacks();
    if (!res) {
        panic("Fatal error parsing track packs: %s", res.error());
    }

    loadTrackDb();
}

std::expected<void, const char *> TrackPackManager::loadTrackPacks() {
    SP_LOG("Loading track packs");

    m_packs.push_back(std::move(TRY(TrackPack::New(vanillaManifest))));
    auto dir = Storage::OpenDir(TRACK_PACK_DIRECTORY);
    if (!dir) {
        SP_LOG("Creating track pack directory");
        Storage::CreateDir(TRACK_PACK_DIRECTORY, true);
        return {};
    }

    std::vector<char> manifestBuf;
    while (auto nodeInfo = dir->read()) {
        if (nodeInfo->type != Storage::NodeType::File) {
            continue;
        }

        SP_LOG("Found track pack '%ls'", nodeInfo->name);
        manifestBuf.resize(nodeInfo->size);

        auto len = Storage::FastReadFile(nodeInfo->id, manifestBuf.data(), nodeInfo->size);
        if (!len.has_value() || *len == 0) {
            SP_LOG("Failed to read track pack manifest!");
            continue;
        }

        manifestBuf.resize(*len);

        auto res = TrackPack::New(std::string_view(manifestBuf.data(), manifestBuf.size()));
        if (!res.has_value()) {
            SP_LOG("Failed to read track pack manifest: %s", res.error());
            continue;
        }

        m_packs.push_back(std::move(*res));
    }

    return {};
}

void TrackPackManager::loadTrackDb() {
    SP_LOG("Loading track DB");

    // Load up the wiimm db, which is pretty large, so we cannot
    // just put it on the stack, so we get the size from stat and
    // allocate a buffer that fits this exact size.
    auto nodeInfo = Storage::Stat(TRACK_DB);
    if (!nodeInfo.has_value()) {
        SP_LOG("No track DB found!");
        return;
    }

    std::string trackDbBuf;
    trackDbBuf.resize(nodeInfo->size);

    auto len = Storage::FastReadFile(nodeInfo->id, trackDbBuf.data(), nodeInfo->size);
    if (!len.has_value() || *len == 0) {
        panic("Failed to read track DB!");
    }

    trackDbBuf.resize(*len);

    Sha1 currentlyParsing;
    bool isSkipping = false;
    IniReader trackDbIni(trackDbBuf);
    while (auto property = trackDbIni.next()) {
        auto [section, key, value] = *property;

        auto sha1 = sha1FromHex(section);
        if (!sha1) {
            SP_LOG("Could not parse sha1!");
            continue;
        }

        if (m_trackDb.empty() || currentlyParsing != *sha1) {
            currentlyParsing = *sha1;
            isSkipping = !anyPackContains(*sha1);
            if (!isSkipping) {
                m_trackDb.emplace_back(*sha1);
                // SP_LOG("Parsed %d tracks", m_trackDb.size());
            }
        }

        if (isSkipping) {
            continue;
        }

        auto &track = m_trackDb.back();
        track.parse(key, value);
    }

    SP_LOG("Finished loading track DB");
}

bool TrackPackManager::anyPackContains(Sha1 sha1) {
    for (const auto &pack : m_packs) {
        if (pack.contains(sha1)) {
            return true;
        }
    }

    return false;
}

const TrackPack &TrackPackManager::getNthPack(u32 n) const {
    return m_packs[n];
}

size_t TrackPackManager::getPackCount() const {
    return m_packs.size();
}

const TrackPack &TrackPackManager::getSelectedTrackPack() const {
    auto *raceConfig = System::RaceConfig::Instance();
    return m_packs[raceConfig->m_selectedTrackPack];
}

const Track &TrackPackManager::getTrack(Sha1 sha1) const {
    for (auto &track : m_trackDb) {
        if (track.m_sha1 == sha1) {
            return track;
        }
    }

    auto *track = getSelectedTrackPack().getUnreleasedTrack(sha1);
    if (track != nullptr) {
        return *track;
    }

    auto hex = sha1ToHex(sha1);
    panic("Unknown sha1 id: %s", hex.data());
}

TrackPackManager &TrackPackManager::Instance() {
    if (s_instance == nullptr) {
        panic("TrackPackManager not initialized");
    }

    return *s_instance;
}

void TrackPackManager::CreateInstance() {
    if (s_instance == nullptr) {
        s_instance = new TrackPackManager;
    }
}

void TrackPackManager::DestroyInstance() {
    delete s_instance;
    s_instance = nullptr;
}

void TrackPackInfo::getTrackPath(char *out, u32 outSize, bool splitScreen) const {
    auto hex = sha1ToHex(m_selectedSha1);
    SP_LOG("Getting track path for %s", hex.data());

    if (System::RaceConfig::Instance()->isVanillaTracks()) {
        auto courseFileName = Registry::courseFilenames[m_selectedCourseId];

        if (splitScreen) {
            snprintf(out, outSize, "Race/Course/%s_d", courseFileName);
        } else {
            snprintf(out, outSize, "Race/Course/%s", courseFileName);
        }

        SP_LOG("Vanilla Track path: %s", out);
    } else {
        snprintf(out, outSize, "/mkw-sp/Tracks/%s", hex.data());
        SP_LOG("Track path: %s", out);
    }
}

u32 TrackPackInfo::getSelectedCourse() const {
    return m_selectedCourseId;
}

const wchar_t *TrackPackInfo::getCourseName() const {
    return m_name.c_str();
}

Sha1 TrackPackInfo::getCourseSha1() const {
    if (System::RaceConfig::Instance()->isVanillaTracks()) {
        auto *saveManager = System::SaveManager::Instance();
        auto myStuffSha1 = saveManager->courseSHA1(m_selectedCourseId);
        if (myStuffSha1.has_value()) {
            return *myStuffSha1;
        }
    }

    return m_selectedSha1;
}

std::optional<u32> TrackPackInfo::getSelectedMusic() const {
    if (m_selectedMusicId == std::numeric_limits<u32>::max()) {
        return std::nullopt;
    } else {
        return m_selectedMusicId;
    }
}

void TrackPackInfo::selectCourse(Sha1 sha) {
    auto &track = TrackPackManager::Instance().getTrack(sha);

    m_name = track.m_name;
    m_selectedSha1 = track.m_sha1;
    m_selectedMusicId = track.m_musicId;
    m_selectedCourseId = track.getCourseId();
}

TrackPackManager *TrackPackManager::s_instance = nullptr;

} // namespace SP