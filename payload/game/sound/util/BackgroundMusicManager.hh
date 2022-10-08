#pragma once

#include "game/sound/SoundId.hh"

#include <nw4r/snd/SoundHandle.hh>

namespace Sound {

class BackgroundMusicManager {
public:
    nw4r::snd::SoundHandle *prepare(SoundId soundId, bool r5);

    static BackgroundMusicManager *Instance();

private:
    u8 _00[0x3c - 0x00];

    static BackgroundMusicManager *s_instance;
};
static_assert(sizeof(BackgroundMusicManager) == 0x3c);

} // namespace Sound
