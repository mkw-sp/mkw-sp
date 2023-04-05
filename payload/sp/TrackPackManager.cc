#include "TrackPackManager.hh"

#include "sp/ThumbnailManager.hh"
#include "sp/settings/IniReader.hh"
#include "sp/storage/Storage.hh"
#include "sp/vanillaTracks.hh"

#include <game/system/RaceConfig.hh>
#include <game/ui/UIControl.hh>
#include <game/util/Registry.hh>
#include <vendor/magic_enum/magic_enum.hpp>

#include <charconv>
#include <cstring>
#include <expected>

#define TRACK_PACK_DIRECTORY L"Track Packs"
#define TRACK_DB L"TrackDB.ini"

using namespace magic_enum::bitwise_operators;

namespace SP {

// clang-format off
u32 Track::getRaceCourseId() const {
    switch (slotId) {
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
    default: panic("Unknown race slot ID: %d", slotId);
    }
}

u32 Track::getBattleCourseId() const {
    switch (slotId) {
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
    default: panic("Unknown battle slot ID: %d", slotId);
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

std::expected<Sha1, const char *> sha1FromSv(std::string_view sv) {
    if (sv.size() != (0x14 * 2)) {
        return std::unexpected("Invalid SHA1 length!");
    }

    Sha1 sha;
    char tByte[3];

    for (u8 i = 0; i < sv.size(); i += 2) {
        tByte[0] = sv[i];
        tByte[1] = sv[i + 1];
        tByte[2] = '\0';

        sha[i / 2] = strtol(tByte, nullptr, 16);
    }

    return sha;
}

u32 Track::getCourseId() const {
    if (isArena) {
        return getBattleCourseId();
    } else {
        return getRaceCourseId();
    }
}

void Track::parse(std::string_view key, std::string_view value) {
    if (key == "trackname") {
        name = value;
    } else if (key == "slot") {
        auto res = u32FromSv(value);
        if (!res) {
            return;
        }

        slotId = *res;
    } else if (key == "type") {
        if (value == "1") {
            isArena = false;
        } else if (value == "2") {
            isArena = true;
        }
    }
}

std::expected<std::vector<Sha1>, const char *> parseTracks(std::string_view tracks) {
    std::vector<Sha1> parsedTrackIds;
    char buf[(sizeof(Sha1) * 2) + 1];
    size_t position = 0;

    for (char c : tracks) {
        if (c == ',' || position == sizeof(buf)) {
            buf[position] = '\0';

            std::string_view sv(buf, position);
            auto trackId = sha1FromSv(sv);
            if (!trackId) {
                return std::unexpected(trackId.error());
            }

            parsedTrackIds.push_back(*trackId);
            position = 0;
        } else if (c != ' ') {
            buf[position] = c;
            position++;
        }
    }

    if (position != 0) {
        std::string_view sv(buf, position);
        auto trackId = sha1FromSv(buf);
        if (!trackId) {
            return std::unexpected(trackId.error());
        }

        parsedTrackIds.push_back(*trackId);
    }

    return parsedTrackIds;
}

TrackPack::TrackPack(std::string_view manifestView) {
    IniReader iniReader(manifestView);

    m_parseError = nullptr;
    bool prettyNameFound = false;
    bool descriptionFound = false;
    bool authorNamesFound = false;
    while (auto property = iniReader.next()) {
        auto [section, key, value] = *property;

        if (section == "Pack Info") {
            if (key == "name") {
                m_prettyName = {value};
                prettyNameFound = true;
            } else if (key == "description") {
                m_description = value;
                descriptionFound = true;
            } else if (key == "author") {
                m_authorNames = value;
                authorNamesFound = true;
            } else if (key == "race") {
                auto res = parseTracks(value);
                if (!res) {
                    m_parseError = res.error();
                    return;
                }

                m_raceTracks = *res;
            } else if (key == "balloon") {
                auto res = parseTracks(value);
                if (!res) {
                    m_parseError = res.error();
                    return;
                }

                m_balloonTracks = *res;
            } else if (key == "coin") {
                auto res = parseTracks(value);
                if (!res) {
                    m_parseError = res.error();
                    return;
                }

                m_coinTracks = *res;
            } else {
                m_parseError = "Unknown key in track pack manifest";
            }
        } else {
            auto sha = sha1FromSv(section);
            if (!sha) {
                m_parseError = sha.error();
                return;
            }

            if (m_unreleasedTracks.size() == 0 || m_unreleasedTracks.back().sha1 != *sha) {
                m_unreleasedTracks.emplace_back(*sha);
            }

            auto &track = m_unreleasedTracks.back();
            track.parse(key, value);
        }
    }

    if (!prettyNameFound || !descriptionFound || !authorNamesFound) {
        m_parseError = "Missing required key in track pack manifest";
    } else if (m_raceTracks.empty() && m_balloonTracks.empty() && m_coinTracks.empty()) {
        m_parseError = "No tracks found in track pack manifest";
    }
}

const Track *TrackPack::getUnreleasedTrack(Sha1 sha1) const {
    for (const auto &track : m_unreleasedTracks) {
        if (track.sha1 == sha1) {
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

TrackGameMode TrackPack::getSupportedModes() const {
    assert(m_parseError == nullptr);
    TrackGameMode modes[] = {
            TrackGameMode::Race,
            TrackGameMode::Coin,
            TrackGameMode::Balloon,
    };

    auto supportedModes = TrackGameMode::None;
    for (auto mode : modes) {
        if (!getTrackList(mode).empty()) {
            supportedModes |= mode;
        }
    }

    return supportedModes;
}

u16 TrackPack::getTrackCount(TrackGameMode mode) const {
    assert(m_parseError == nullptr);
    return getTrackList(mode).size();
}

std::optional<Sha1> TrackPack::getNthTrack(u32 n, TrackGameMode mode) const {
    assert(m_parseError == nullptr);

    auto &trackList = getTrackList(mode);
    if (trackList.size() <= n) {
        return std::nullopt;
    } else {
        return trackList[n];
    }
}

const char *TrackPack::getParseError() const {
    return m_parseError;
}

const wchar_t *TrackPack::getPrettyName() const {
    assert(m_parseError == nullptr);
    return m_prettyName.c_str();
}

TrackPackManager::TrackPackManager() {
    loadTrackPacks();
    loadTrackDb();
}

void TrackPackManager::loadTrackPacks() {
    SP_LOG("Loading track packs");

    m_packs.emplace_back(vanillaManifest);
    auto dir = Storage::OpenDir(TRACK_PACK_DIRECTORY);
    if (!dir) {
        SP_LOG("Creating track pack directory");
        Storage::CreateDir(TRACK_PACK_DIRECTORY, true);
        return;
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

        auto pack = TrackPack(manifestBuf);
        auto parseError = pack.getParseError();

        if (parseError != nullptr) {
            SP_LOG("Failed to read track pack manifest: %s", parseError);
            continue;
        }

        m_packs.push_back(std::move(pack));
    }
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

    IniReader trackDbIni(trackDbBuf);
    while (auto property = trackDbIni.next()) {
        auto [section, key, value] = *property;

        auto sha1 = sha1FromSv(section);
        if (!sha1) {
            SP_LOG("Could not parse sha1!");
            continue;
        }

        if (m_trackDb.size() == 0 || m_trackDb.back().sha1 != *sha1) {
            m_trackDb.emplace_back(*sha1);
        }

        auto &track = m_trackDb.back();
        track.parse(key, value);
    }

    SP_LOG("Finished loading track DB");
}

const TrackPack &TrackPackManager::getNthPack(u32 n) const {
    return m_packs[n];
}

size_t TrackPackManager::getPackCount() const {
    return m_packs.size();
}

const TrackPack &TrackPackManager::getSelectedTrackPack() const {
    auto &trackPackInfo = System::RaceConfig::Instance()->m_packInfo;
    return m_packs[trackPackInfo.m_selectedTrackPack];
}

void TrackPackInfo::setTrackMessage(UI::LayoutUIControl *control) const {
    return setTrackMessage(control, s_name.c_str(), m_selectedCourseId);
}

void TrackPackInfo::setTrackMessage(UI::LayoutUIControl *control, const wchar_t *name,
        u32 courseId) const {
    auto raceConfig = System::RaceConfig::Instance();

    if (isVanilla()) {
        if (raceConfig->menuScenario().gameMode == System::RaceConfig::GameMode::OfflineBT) {
            control->setMessageAll(9400 + courseId - 32);
        } else {
            control->setMessageAll(9360 + courseId);
        }

        return;
    }

    UI::MessageInfo info;
    info.strings[0] = name;
    control->setMessageAll(20031, &info);
}

const Track &TrackPackManager::getTrack(Sha1 sha1) const {
    for (auto &track : m_trackDb) {
        if (track.sha1 == sha1) {
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
    SP_LOG("Getting track path for %s", hex);

    if (isVanilla()) {
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

bool TrackPackInfo::isVanilla() const {
    return m_selectedTrackPack == 0;
}

u32 TrackPackInfo::getSelectedCourse() const {
    return m_selectedCourseId;
}

Sha1 TrackPackInfo::getSelectedSha1() const {
    return m_selectedSha1;
}

void TrackPackInfo::selectCourse(Sha1 sha) {
    auto &trackPackManager = TrackPackManager::Instance();
    auto &track = trackPackManager.getTrack(sha);

    s_name = track.name;
    m_selectedSha1 = track.sha1;
    m_selectedCourseId = track.getCourseId();

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.courseId = m_selectedCourseId;
}

WFixedString<64> TrackPackInfo::s_name = {};
TrackPackManager *TrackPackManager::s_instance = nullptr;

} // namespace SP
