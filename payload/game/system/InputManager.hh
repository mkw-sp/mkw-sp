#pragma once

#include <Common.hh>

namespace System {

namespace Button {

enum {
    Accel = 1 << 0,
    Brake = 1 << 1,
    Item = 1 << 2,
    Drift = 1 << 3,
    BrakeDrift = 1 << 4, // Added
    LookBackwards = 1 << 5,
};

} // namespace Buttons

struct RaceInputState {
    void *_00;
    u16 buttons;
    u16 rawButtons;
    Vec2 stick;
    u8 rawStick[2];
    u8 trick;
    u8 rawTrick;
    bool isValid : 1;
};
static_assert(sizeof(RaceInputState) == 0x18);

class PadProxy {
public:
    const RaceInputState &currentRaceInputState() const;

private:
    u8 _00[0x28 - 0x00];
    RaceInputState m_currentRaceInputState;
    RaceInputState m_lastRaceInputState;
    u8 _58[0xd8 - 0x58];
};
static_assert(sizeof(PadProxy) == 0xd8);

} // namespace System
