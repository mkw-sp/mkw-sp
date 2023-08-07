#pragma once

extern "C" {
#include <revolution/pad.h>
}

#include "game/util/Registry.hh"

#include <sp/CircularBuffer.hh>

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
    RaceInputState();
    virtual ~RaceInputState();
    virtual void dt(s32 type);

    static void Reset(RaceInputState &state);
    static void SetStickX(RaceInputState &state, u8 stickX);
    static void SetStickY(RaceInputState &state, u8 stickY);
    static void SetTrick(RaceInputState &state, u8 trick);

    u16 _04 : 10;
    bool lookBackwards : 1;
    bool brakeDrift : 1;
    bool drift : 1;
    bool item : 1;
    bool brake : 1;
    bool accelerate : 1;
    u16 rawButtons;
    Vec2<f32> stick;
    Vec2<u8> rawStick;
    u8 trick;
    u8 rawTrick;
    bool isValid : 1;
};
static_assert(sizeof(RaceInputState) == 0x18);

struct UIInputState {
    u8 _00[0x08 - 0x00];
    Vec2<f32> stick;
    u8 _10[0x34 - 0x10];
};
static_assert(sizeof(UIInputState) == 0x34);

class Pad {
public:
    Pad();
    virtual ~Pad();
    virtual void dt(s32 type);

protected:
    virtual void process(RaceInputState &raceInputState, UIInputState &uiInputState) = 0;

public:
    virtual Registry::Controller getControllerId() const;
    virtual void vf_14();
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();
    virtual void vf_24();
    virtual void vf_28();
    virtual void vf_2c();
    virtual void vf_30();
    virtual void vf_34();
    virtual void vf_38();
    virtual void vf_3c();
    virtual void vf_40();
    virtual void vf_44();

protected:
    void processSimplified(RaceInputState &raceInputState, bool isPressed);

public:
    void calc();

private:
    RaceInputState m_currentRaceInputState;
    UIInputState m_currentUIInputState;
    u8 _50[0x90 - 0x50];
};
static_assert(sizeof(Pad) == 0x90);

class DummyPad : public Pad {
public:
    DummyPad();
    ~DummyPad() override;

    void process(RaceInputState &raceInputState, UIInputState &uiInputState) override;
};

class UserPad : public Pad {
public:
    UserPad();
    ~UserPad();

    void process(RaceInputState &raceInputState, UIInputState &uiInputState) override;
    Registry::Controller getControllerId() const override;

    RaceInputState m_userInputState;
};
static_assert(sizeof(UserPad) == 0xa8);

class WiiPad : public Pad {
public:
    WiiPad();
    ~WiiPad() override;

    void REPLACED(process)(RaceInputState &raceInputState, UIInputState &uiInputState);
    REPLACE void process(RaceInputState &raceInputState, UIInputState &uiInputState) override;

protected:
    void REPLACED(
            processClassic)(void *r4, RaceInputState &raceInputState, UIInputState &uiInputState);
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

class GhostPad : public Pad {
public:
    GhostPad();
    ~GhostPad() override;

protected:
    void REPLACED(process)(RaceInputState &raceInputState, UIInputState &uiInputState);
    REPLACE void process(RaceInputState &raceInputState, UIInputState &uiInputState) override;

private:
    u8 _90[0xa8 - 0x90];
};
static_assert(sizeof(GhostPad) == 0xa8);

class PadProxy {
public:
    PadProxy();
    virtual ~PadProxy();
    virtual void dt(s32 type);
    virtual void calc(bool isPaused);
    virtual void reset();
    virtual void setRaceInputState(const RaceInputState &inputState);
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();

    const Pad *pad() const;
    const RaceInputState &currentRaceInputState() const;
    void setPad(Pad *pad, Pad *r5);

private:
    Pad *m_pad;
    u8 _08[0x28 - 0x08];
    RaceInputState m_currentRaceInputState;
    RaceInputState m_lastRaceInputState;
    u8 _58[0xd8 - 0x58];
};
static_assert(sizeof(PadProxy) == 0xd8);

class GhostPadProxy : public PadProxy {
public:
    GhostPadProxy();
    ~GhostPadProxy() override;
    void dt(s32 type) override;
    void calc(bool isPaused) override;
    void reset() override;
    void vf_18() override;
    void vf_1c() override;

    void setPad(GhostPad *pad, const void *inputs, bool driftIsAuto);
    void init();
    void start();
    void end();

private:
    u8 _d8[0xe4 - 0xd8];
    bool m_isLocked;
    u8 _e5[0xec - 0xe5];
};

class UserPadProxy : public PadProxy {
public:
    UserPadProxy();
    ~UserPadProxy() override;
    void dt(s32 type) override;
    void setRaceInputState(const RaceInputState &inputState) override;

private:
    UserPad m_userPad;
};
static_assert(sizeof(UserPadProxy) == 0x180);

class PadRollback {
public:
    PadRollback();

    void calc(u32 playerId);
    void reset();

private:
    struct Frame {
        u32 time;
        RaceInputState inputState;
    };

    std::optional<Frame> serverFrame(u32 playerId) const;
    void handleFutureFrame(const Frame &frame);
    void handlePastFrame(const Frame &frame);
    void applyFrame(u32 playerId, const Frame &frame);

    u32 m_playerId;
    SP::CircularBuffer<Frame, 60> m_frames;
};

class InputManager {
public:
    bool isMirror() const;
    GhostPadProxy *ghostProxy(u32 i);
    UserPadProxy *userProxy(u32 i);
    const PADStatus &padStatus(u32 i) const;
    UserPad *extraUserPad(u32 i);
    GhostPadProxy *extraGhostProxy(u32 i);

    void setExtraUserPad(u32 i);
    void setGhostPad(u32 i, const void *ghostInputs, bool driftIsAuto);

    void REPLACED(reset)();
    REPLACE void reset();
    void REPLACED(calcPads)(bool isPaused);
    REPLACE void calcPads(bool isPaused);
    void REPLACED(calc)();
    REPLACE void calc();
    void REPLACED(initGhostProxies)();
    REPLACE void initGhostProxies();
    void REPLACED(startGhostProxies)();
    REPLACE void startGhostProxies();
    void endExtraGhostProxy(u32 playerId);
    void REPLACED(endGhostProxies)();
    REPLACE void endGhostProxies();
    void calcRollbacks();

    static REPLACE InputManager *CreateInstance();
    static InputManager *Instance();

private:
    InputManager();

    u8 _0000[0x0004 - 0x0000];
    GhostPadProxy m_ghostProxies[4];
    UserPadProxy m_userProxies[12];
    u8 _15b4[0x1690 - 0x15b4];
    DummyPad m_dummyPad;
    u8 _1720[0x4120 - 0x1720];
    PADStatus m_padStatus[4];
    u8 _4150[0x4154 - 0x4150];
    bool m_isPaused;
    bool m_isMirror;
    u8 _4156[0x415c - 0x4156];
    GhostPad *m_extraGhostPads;         // Added
    GhostPadProxy *m_extraGhostProxies; // Added
    PadRollback *m_rollbacks;           // Added

    static InputManager *s_instance;
};
static_assert(sizeof(InputManager) == 0x415c + sizeof(void *) * 3);

} // namespace System
