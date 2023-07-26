#pragma once

#include "game/sound/SoundId.hh"

#include <Common.hh>

namespace Sound {

class RaceSoundManager {
public:
    u32 state() const;
    void transition(u32 state);

    SoundId REPLACED(getBGMSoundId)() const;
    REPLACE SoundId getBGMSoundId() const;

    static RaceSoundManager *Instance();

private:
    u8 _000[0x040 - 0x000];
    u32 m_state;
    u8 _044[0x178 - 0x000];

    static RaceSoundManager *s_instance;
};

} // namespace Sound
