#pragma once

#include "sp/FixedString.hh"
#include "sp/ShaUtil.hh"

#include <game/sound/SoundId.hh>
#include <game/util/Registry.hh>

#include <limits>
#include <optional>
#include <span>

namespace System {
class RaceConfig;
}

namespace SP {

class Track {
public:
    enum class Mode {
        Race = 1 << 0,
        Balloon = 1 << 1,
        Coin = 1 << 2,
    };

    static Track FromFile(std::span<u8> manifestBuf, Sha1 sha1);
    void applyToConfig(System::RaceConfig *raceConfig, bool inRace) const;

    Sha1 m_sha1;
    Registry::Course m_courseId;
    std::optional<Sound::SoundId> m_musicId;
    WFixedString<48> m_name = {};

private:
    Track() = default;
};

constexpr Track::Mode s_trackModes[] = {
        Track::Mode::Race,
        Track::Mode::Coin,
        Track::Mode::Balloon,
};

} // namespace SP
