#include "Track.hh"

#include <game/system/RaceConfig.hh>
#include <game/ui/SectionManager.hh>
#include <protobuf/TrackPacks.pb.h>
#include <vendor/nanopb/pb_decode.h>

#include <cstdio>

namespace SP {

Track Track::FromFile(std::span<u8> manifestBuf, Sha1 sha1) {
    ProtoTrack protoTrack;
    pb_istream_t stream = pb_istream_from_buffer(manifestBuf.data(), manifestBuf.size());
    if (!pb_decode(&stream, ProtoTrack_fields, &protoTrack)) {
        panic("Failed to decode track: %s", PB_GET_ERROR(&stream));
    }

    Track self;
    self.m_sha1 = sha1;
    self.m_courseId = static_cast<Registry::Course>(protoTrack.courseId);

    auto nameSize = strnlen(protoTrack.name, 0x48);
    self.m_name.setUTF8(std::string_view(protoTrack.name, nameSize));

    if (protoTrack.has_musicId) {
        self.m_musicId = static_cast<Sound::SoundId>(protoTrack.musicId);
    }

    return self;
}

void Track::applyToConfig(System::RaceConfig *raceConfig, bool inRace) const {
    System::RaceConfig::Scenario *scenario;
    System::RaceConfig::SPScenario *spScenario;
    if (inRace) {
        spScenario = &raceConfig->m_spRace;
        scenario = &raceConfig->m_raceScenario;
    } else {
        spScenario = &raceConfig->m_spMenu;
        scenario = &raceConfig->m_menuScenario;
    }

    scenario->courseId = m_courseId;

    auto *globalContext = UI::SectionManager::Instance()->globalContext();
    if (globalContext->isVanillaTracks()) {
        spScenario->nameReplacement.m_len = 0;
        spScenario->pathReplacement.m_len = 0;
        spScenario->courseSha = std::nullopt;
        spScenario->musicReplacement = std::nullopt;
        return;
    }

    spScenario->courseSha = m_sha1;
    spScenario->nameReplacement = m_name;
    spScenario->musicReplacement = m_musicId;

    auto hex = sha1ToHex(m_sha1);
    spScenario->pathReplacement.m_len = snprintf(spScenario->pathReplacement.m_buf.data(), 64,
            "Tracks/%s.arc.lzma", hex.data());
}

} // namespace SP
