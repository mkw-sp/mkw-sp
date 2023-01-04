#pragma once

#include "game/ui/ctrl/CtrlRaceBase.hh"

#include <vendor/magic_enum/magic_enum.hpp>

namespace UI {

class CtrlRaceInputDisplay : public CtrlRaceBase {
public:
    CtrlRaceInputDisplay();
    ~CtrlRaceInputDisplay() override;
    void draw() override;
    void initSelf() override;
    void calcSelf() override;

    void load(u32 localPlayerCount, u32 localPlayerId);

private:
    enum class DpadState {
        // The order here must match the order in the `trick` field of RaceInputState
        Off,
        Up,
        Down,
        Left,
        Right,
    };

    enum class AccelState {
        Off,
        Pressed,
    };

    enum class Trigger {
        L,
        R,
        BD, // 200cc only
    };

    enum class TriggerState {
        Off,
        Pressed,
    };

    void setDpad(DpadState state);
    void setAccel(AccelState state);
    void setTrigger(Trigger trigger, TriggerState state);
    void setStick(Vec2<f32> state);

    static constexpr u32 dpadStateCount = magic_enum::enum_count<DpadState>();
    static constexpr u32 accelStateCount = magic_enum::enum_count<DpadState>();
    static constexpr u32 triggerCount = magic_enum::enum_count<DpadState>();
    static constexpr u32 triggerStateCount = magic_enum::enum_count<DpadState>();

    nw4r::lyt::Pane *m_dpadPanes[dpadStateCount];
    nw4r::lyt::Pane *m_accelPanes[accelStateCount];
    nw4r::lyt::Pane *m_triggerPanes[triggerCount][triggerStateCount];
    nw4r::lyt::Pane *m_stickPane;
    Vec3<f32> m_stickOrigin;

    DpadState m_dpadState = DpadState::Off;
    s8 m_dpadTimer = 0; // Hold the DPAD press for DPAD_HOLD_FOR_N_FRAMES frames
    AccelState m_accelState = AccelState::Off;
    TriggerState m_triggerStates[triggerCount]{};
    Vec2<f32> m_stickState{ 0.0f, 0.0f };

    u32 m_playerId;

    static const s8 DPAD_HOLD_FOR_N_FRAMES;
};

} // namespace UI
