#pragma once

#include <Common.hh>

namespace Sound {

class DriverSoundManager {
public:
    void playSelectSound(u32 localPlayerId, s32 characterId);

    static DriverSoundManager *Instance();

private:
    u8 _0000[0x4758 - 0x0000];

    static DriverSoundManager *s_instance;
};
static_assert(sizeof(DriverSoundManager) == 0x4758);

} // namespace Sound
