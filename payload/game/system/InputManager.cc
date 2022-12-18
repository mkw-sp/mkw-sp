#include "InputManager.hh"

#include "game/system/SaveManager.hh"

extern "C" {
#include <revolution/kpad.h>
}
#include <sp/cs/RaceClient.hh>
#include <sp/cs/RaceServer.hh>

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

    if (std::abs(raceInputState.rawStick.x - 7) <= 2 * std::abs(raceInputState.rawStick.y - 7)) {
        raceInputState.rawTrick = raceInputState.rawStick.y < 7 ? Trick::Down : Trick::Up;
    } else {
        raceInputState.rawTrick = raceInputState.rawStick.x < 7 ? Trick::Left : Trick::Right;
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

const Pad *PadProxy::pad() const {
    return m_pad;
}

const RaceInputState &PadProxy::currentRaceInputState() const {
    return m_currentRaceInputState;
}

void GhostPadProxy::init() {
    m_isLocked = true;
}

bool InputManager::isMirror() const {
    return m_isMirror;
}

GhostPadProxy *InputManager::ghostProxy(u32 i) {
    return &m_ghostProxies[i];
}

GhostPadProxy *InputManager::extraGhostProxy(u32 i) {
    return &m_extraGhostProxies[i];
}

void InputManager::setGhostPad(u32 i, const void *ghostInputs, bool driftIsAuto) {
    m_extraGhostProxies[i].setPad(&m_extraGhostPads[i], ghostInputs, driftIsAuto);
}

void InputManager::reset() {
    for (u32 i = 0; i < 12; i++) {
        m_extraGhostProxies[i].reset();
    }

    REPLACED(reset)();
}

void InputManager::calcPads(bool isPaused) {
    REPLACED(calcPads)(isPaused);

    if (!isPaused) {
        for (u32 i = 0; i < 12; i++) {
            m_extraGhostPads[i].calc();
        }
    }
}

void InputManager::calc() {
    if (auto *raceServer = SP::RaceServer::Instance()) {
        raceServer->calcRead();
    }

    REPLACED(calc)();

    for (u32 i = 0; i < 12; i++) {
        m_extraGhostProxies[i].calc(m_isPaused);
    }

    if (auto *raceClient = SP::RaceClient::Instance()) {
        raceClient->calcWrite();
    }
}

void InputManager::initGhostProxies() {
    REPLACED(initGhostProxies)();

    for (u32 i = 0; i < 12; i++) {
        m_extraGhostProxies[i].init();
    }
}

void InputManager::startGhostProxies() {
    REPLACED(startGhostProxies)();

    for (u32 i = 0; i < 12; i++) {
        m_extraGhostProxies[i].start();
    }
}

void InputManager::endExtraGhostProxy(u32 playerId) {
    m_extraGhostProxies[playerId].end();
}

void InputManager::endGhostProxies() {
    REPLACED(endGhostProxies)();

    for (u32 i = 0; i < 12; i++) {
        m_extraGhostProxies[i].end();
    }
}

InputManager *InputManager::CreateInstance() {
    s_instance = new InputManager;
    assert(s_instance);

    s_instance->m_extraGhostPads = new GhostPad[12];
    s_instance->m_extraGhostProxies = new GhostPadProxy[12];
    for (u32 i = 0; i < 12; i++) {
        s_instance->m_extraGhostProxies[i].PadProxy::setPad(&s_instance->m_dummyPad, nullptr);
    }

    return s_instance;
}

InputManager *InputManager::Instance() {
    return s_instance;
}

} // namespace System
