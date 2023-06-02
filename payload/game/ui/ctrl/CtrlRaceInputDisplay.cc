#include "CtrlRaceInputDisplay.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include <cstdio>

namespace UI {

CtrlRaceInputDisplay::CtrlRaceInputDisplay() = default;

CtrlRaceInputDisplay::~CtrlRaceInputDisplay() = default;

void CtrlRaceInputDisplay::draw(int pass) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::InputDisplay>();
    if (setting != SP::ClientSettings::InputDisplay::Simple) {
        return;
    }

    LayoutUIControl::draw(pass);
}

void CtrlRaceInputDisplay::initSelf() {
    CtrlRaceBase::initSelf();

    // Note: enum_name returns a string_view over a null-terminated string, thus the conversion in
    // the other direction is safe.
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    magic_enum::enum_for_each<DpadState>([&](auto s) {
        DpadState state = s;
        auto stateName = magic_enum::enum_name(state);
        char name[0x20];
        snprintf(name, sizeof(name), "Dpad%.*s", stateName.length(), stateName.data());
        auto *pane = m_mainLayout.findPaneByName(name);
        assert(pane);
        pane->m_visible = state == DpadState::Off;
        m_dpadPanes[static_cast<u32>(state)] = pane;
    });

    magic_enum::enum_for_each<AccelState>([&](auto s) {
        AccelState state = s;
        auto stateName = magic_enum::enum_name(state);
        char name[0x20];
        snprintf(name, sizeof(name), "Accel%.*s", stateName.length(), stateName.data());
        auto *pane = m_mainLayout.findPaneByName(name);
        assert(pane);
        pane->m_visible = state == AccelState::Off;
        if (raceScenario.is200cc) {
            pane->m_trans.x += pane->m_scale.x * 15.0f;
            pane->m_trans.y += pane->m_scale.y * 15.0f;
        }
        m_accelPanes[static_cast<u32>(state)] = pane;
    });

    magic_enum::enum_for_each<Trigger>([&](auto t) {
        Trigger trigger = t;
        auto triggerName = magic_enum::enum_name(trigger);
        magic_enum::enum_for_each<TriggerState>([&](auto s) {
            TriggerState state = s;
            auto stateName = magic_enum::enum_name(state);
            char name[0x20];
            snprintf(name, sizeof(name), "Trigger%.*s%.*s", triggerName.length(),
                    triggerName.data(), stateName.length(), stateName.data());
            auto *pane = m_mainLayout.findPaneByName(name);
            assert(pane);
            pane->m_visible = state == TriggerState::Off;
            if (!raceScenario.is200cc && trigger == Trigger::BD) {
                pane->m_visible = false;
            }
            m_triggerPanes[static_cast<u32>(trigger)][static_cast<u32>(state)] = pane;
        });
    });

    m_stickPane = m_mainLayout.findPaneByName("Stick");
    assert(m_stickPane);
    m_stickOrigin = m_stickPane->m_trans;

    m_playerId = getPlayerId();
}

void CtrlRaceInputDisplay::calcSelf() {
    process();

    u32 playerId = getPlayerId();
    if (playerId != m_playerId) {
        m_dpadTimer = 0;
        m_playerId = playerId;
    }

    System::RaceInputState input{};
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (playerId < System::RaceConfig::Instance()->raceScenario().playerCount) {
        auto *player = System::RaceManager::Instance()->player(playerId);
        input = player->padProxy()->currentRaceInputState();
        if (!input.isValid) {
            input = {};
        }
    }

    assert(input.trick < magic_enum::enum_count<DpadState>());
    assert(input.stick.x <= 1.0f && input.stick.x >= -1.0f);
    assert(input.stick.y <= 1.0f && input.stick.y >= -1.0f);

    auto dpad = static_cast<DpadState>(input.trick);

    // Mirror mode inverts stick and dpad inputs
    if (raceScenario.mirror) {
        if (dpad == DpadState::Left) {
            dpad = DpadState::Right;
        } else if (dpad == DpadState::Right) {
            dpad = DpadState::Left;
        }

        input.stick.x *= -1.0f;
    }

    setDpad(dpad);
    setAccel(input.accelerate ? AccelState::Pressed : AccelState::Off);

    bool l = input.item;
    setTrigger(Trigger::L, l ? TriggerState::Pressed : TriggerState::Off);
    bool r = input.brake || input.drift;
    setTrigger(Trigger::R, r ? TriggerState::Pressed : TriggerState::Off);
    setStick(input.stick);

    if (raceScenario.is200cc) {
        bool brakeDrift = input.brakeDrift;
        setTrigger(Trigger::BD, brakeDrift ? TriggerState::Pressed : TriggerState::Off);
    }
}

void CtrlRaceInputDisplay::setDpad(DpadState state) {
    if (state == m_dpadState) {
        return;
    }

    // Only hold for off press
    if (state == DpadState::Off && m_dpadTimer != 0 && --m_dpadTimer) {
        return;
    }

    m_dpadPanes[static_cast<u32>(m_dpadState)]->m_visible = false;
    m_dpadPanes[static_cast<u32>(state)]->m_visible = true;
    m_dpadState = state;
    m_dpadTimer = DPAD_HOLD_FOR_N_FRAMES;
}

void CtrlRaceInputDisplay::setAccel(AccelState state) {
    if (state == m_accelState) {
        return;
    }

    m_accelPanes[static_cast<u32>(m_accelState)]->m_visible = false;
    m_accelPanes[static_cast<u32>(state)]->m_visible = true;
    m_accelState = state;
}

void CtrlRaceInputDisplay::setTrigger(Trigger trigger, TriggerState state) {
    auto t = static_cast<u32>(trigger);
    if (state == m_triggerStates[t]) {
        return;
    }

    m_triggerPanes[t][static_cast<u32>(m_triggerStates[t])]->m_visible = false;
    m_triggerPanes[t][static_cast<u32>(state)]->m_visible = true;
    m_triggerStates[t] = state;
}

void CtrlRaceInputDisplay::setStick(Vec2<f32> state) {
    if (state.x == m_stickState.x && state.y == m_stickState.y) {
        return;
    }

    // Map range [-1, 1] -> [-width * 5 / 19, width * 5 / 19]
    f32 scale = 5.0f / 19.0f;
    m_stickPane->m_trans.x =
            m_stickOrigin.x + scale * state.x * m_stickPane->m_scale.x * m_stickPane->m_width;
    m_stickPane->m_trans.y =
            m_stickOrigin.y + scale * state.y * m_stickPane->m_scale.y * m_stickPane->m_height;

    m_stickState = state;
}

void CtrlRaceInputDisplay::load(u32 localPlayerCount, u32 localPlayerId) {
    m_localPlayerId = localPlayerId;

    char variant[0x20];
    u32 variantId = localPlayerCount == 3 ? 4 : localPlayerCount;
    snprintf(variant, sizeof(variant), "InputDisplay_%u_%u", variantId, localPlayerId);

    const char *groups[] = {nullptr, nullptr};
    LayoutUIControl::load("game_image", "InputDisplay", variant, groups);
}

const s8 CtrlRaceInputDisplay::DPAD_HOLD_FOR_N_FRAMES = 10;

} // namespace UI
