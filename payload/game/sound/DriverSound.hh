#pragma once

#include <Common.hh>

namespace Sound {

class DriverSound {
public:
    void REPLACED(onIndirectHit)(u32 receiver);
    REPLACE void onIndirectHit(u32 receiver);

private:
    u8 _000[0x6ec - 0x000];
    u8 m_playerId;
    u8 _6ed[0x708 - 0x6ed];
};
static_assert(sizeof(DriverSound) == 0x708);

} // namespace Sound
