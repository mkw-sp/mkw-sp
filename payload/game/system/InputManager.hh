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

} // namespace Button

namespace Trick {

enum {
    Off,
    Up,
    Down,
    Left,
    Right,
};

} // namespace Trick

struct RaceInputState {
    void *_00;
    u16 buttons;
    u16 rawButtons;
    Vec2<f32> stick;
    Vec2<u8> rawStick;
    u8 trick;
    u8 rawTrick;
    bool isValid : 1;
};
static_assert(sizeof(RaceInputState) == 0x18);

struct UIInputState {
    u8 _00[0x34 - 0x00];
};
static_assert(sizeof(UIInputState) == 0x34);

class Pad {
public:
    Pad();
    virtual ~Pad();
    virtual void dt(s32 type);

protected:
    virtual void process(RaceInputState &raceInputState, UIInputState &uiInputState) = 0;
    // ...

    void processSimplified(RaceInputState &raceInputState, bool isPressed);

private:
    RaceInputState m_currentRaceInputState;
    UIInputState m_currentUIInputState;
    u8 _50[0x90 - 0x50];
};
static_assert(sizeof(Pad) == 0x90);

class WiiPad : public Pad {
public:
    WiiPad();
    ~WiiPad() override;

protected:
    void REPLACED(processClassic)(void *r4, RaceInputState &raceInputState,
            UIInputState &uiInputState);
    REPLACE void processClassic(void *r4, RaceInputState &raceInputState,
            UIInputState &uiInputState);

private:
    u8 _920[0x920 - 0x90];
};
static_assert(sizeof(WiiPad) == 0x920);

class GCPad : public Pad {
public:
    GCPad();
    ~GCPad() override;

protected:
    void REPLACED(process)(RaceInputState &raceInputState, UIInputState &uiInputState);
    REPLACE void process(RaceInputState &raceInputState, UIInputState &uiInputState) override;

private:
    u8 _90[0xb0 - 0x90];
};
static_assert(sizeof(GCPad) == 0xb0);

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

class InputManager {
public:
    bool isMirror() const;

    static InputManager *Instance();

private:
    u8 _0000[0x4155 - 0x0000];
    bool m_isMirror;
    u8 _4156[0x415c - 0x4156];

    static InputManager *s_instance;
};
static_assert(sizeof(InputManager) == 0x415c);

} // namespace System
