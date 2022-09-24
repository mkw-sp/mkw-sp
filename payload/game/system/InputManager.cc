#include "InputManager.hh"

#include "game/system/SaveManager.hh"

extern "C" {
#include <revolution.h>
#include <revolution/kpad.h>
}

#include <cmath>

namespace System {

void Pad::processSimplified(RaceInputState &raceInputState, bool isPressed) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::SimplifiedControls>();
    switch (setting) {
    case SP::ClientSettings::SimplifiedControls::Off:
        return;
    case SP::ClientSettings::SimplifiedControls::NonExclusive:
        if (raceInputState.rawTrick != 0) {
            return;
        }
        break;
    case SP::ClientSettings::SimplifiedControls::Exclusive:
        raceInputState.rawTrick = 0;
        raceInputState.trick = 0;
        break;
    }

    if (!isPressed) {
        return;
    }

    SPFooter::OnSimplifiedControls();

    if (raceInputState.rawStick.y < 7) {
        raceInputState.rawTrick = Trick::Down;
    } else {
        raceInputState.rawTrick = Trick::Up;
    }

    if (std::abs(raceInputState.rawStick.x - 7) > std::abs(raceInputState.rawStick.y - 7)) {
        if (raceInputState.rawStick.x < 7) {
            raceInputState.rawTrick = Trick::Left;
        } else {
            raceInputState.rawTrick = Trick::Right;
        }
    }

    bool isMirror = InputManager::Instance()->isMirror();
    if (isMirror && raceInputState.rawTrick == Trick::Left) {
        raceInputState.trick = Trick::Right;
    } else if (isMirror && raceInputState.rawTrick == Trick::Right) {
        raceInputState.trick = Trick::Left;
    } else {
        raceInputState.trick = raceInputState.rawTrick;
    }
}

void WiiPad::processClassic(void *r4, RaceInputState &raceInputState, UIInputState &uiInputState) {
    REPLACED(processClassic)(r4, raceInputState, uiInputState);

    processSimplified(raceInputState, raceInputState.rawButtons & KPAD_CL_TRIGGER_ZL);
}

void GCPad::process(RaceInputState &raceInputState, UIInputState &uiInputState) {
    REPLACED(process)(raceInputState, uiInputState);

    processSimplified(raceInputState, raceInputState.rawButtons & PAD_BUTTON_Y);
}

const RaceInputState &PadProxy::currentRaceInputState() const {
    return m_currentRaceInputState;
}

bool InputManager::isMirror() const {
    return m_isMirror;
}

InputManager *InputManager::Instance() {
    return s_instance;
}

} // namespace System
