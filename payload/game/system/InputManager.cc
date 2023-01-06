#include "InputManager.hh"

#include "game/gfx/CameraManager.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

extern "C" {
#include <revolution/kpad.h>
}
#include <sp/cs/RaceClient.hh>
#include <sp/cs/RaceServer.hh>

#include <cmath>

namespace System {

RaceInputState::RaceInputState() {
    Reset(*this);
}

RaceInputState::~RaceInputState() = default;

Pad::~Pad() = default;

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

UserPad::UserPad() = default;

UserPad::~UserPad() = default;

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

PadRollback::PadRollback() = default;

void PadRollback::calc(u32 playerId) {
    auto *raceClient = SP::RaceClient::Instance();
    if (!raceClient->roomManager().isPlayerRemote(playerId)) {
        return;
    }

    u32 frameId = RaceManager::Instance()->frameId();
    auto *proxy = System::InputManager::Instance()->userProxy(playerId);
    if (auto serverFrame = raceClient->frame()) {
        s32 delay = static_cast<s32>(frameId) - static_cast<s32>(serverFrame->id);
        auto &framePlayer = serverFrame->players[playerId];
        RaceInputState inputState;
        inputState.accelerate = framePlayer.inputState.accelerate;
        inputState.brake = framePlayer.inputState.brake;
        inputState.item = false;
        inputState.drift = framePlayer.inputState.drift;
        inputState.brakeDrift = framePlayer.inputState.brakeDrift; // TODO check for 200cc
        System::RaceInputState::SetStickX(inputState, framePlayer.inputState.stickX);
        System::RaceInputState::SetStickY(inputState, framePlayer.inputState.stickY);
        System::RaceInputState::SetTrick(inputState, framePlayer.inputState.trick);
        if (delay <= 0) {
            while (m_frames.front() && m_frames.front()->id < frameId) {
                m_frames.pop();
            }
            if (!m_frames.full()) {
                m_frames.push({serverFrame->id, inputState});
            }
        } else {
            while (m_frames.front() && m_frames.front()->id < serverFrame->id) {
                m_frames.pop();
            }
            auto *rollbackFrame = m_frames.front();
            if (rollbackFrame && rollbackFrame->id == serverFrame->id) {
                for (u32 i = 0; i < m_frames.count(); i++) {
                    m_frames[i]->inputState = inputState;
                }
            }
        }
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (m_frames[i]->id == frameId - 1) {
                proxy->setRaceInputState(m_frames[i]->inputState);
                break;
            }
        }
    }

    if (!m_frames.back() || m_frames.back()->id < frameId) {
        if (m_frames.full()) {
            m_frames.pop();
        }
        m_frames.push({frameId, proxy->currentRaceInputState()});
    }
}

void PadRollback::reset() {
    m_frames.reset();
}

bool InputManager::isMirror() const {
    return m_isMirror;
}

GhostPadProxy *InputManager::ghostProxy(u32 i) {
    return &m_ghostProxies[i];
}

UserPadProxy *InputManager::userProxy(u32 i) {
    return &m_userProxies[i];
}

UserPad *InputManager::extraUserPad(u32 i) {
    return &m_extraUserPads[i];
}

GhostPadProxy *InputManager::extraGhostProxy(u32 i) {
    return &m_extraGhostProxies[i];
}

void InputManager::setExtraUserPad(u32 i) {
    m_extraGhostProxies[i].PadProxy::setPad(&m_extraUserPads[i], nullptr);
}

void InputManager::setGhostPad(u32 i, const void *ghostInputs, bool driftIsAuto) {
    m_extraGhostProxies[i].setPad(&m_extraGhostPads[i], ghostInputs, driftIsAuto);
}

void InputManager::reset() {
    for (u32 i = 0; i < 12; i++) {
        m_extraGhostProxies[i].reset();
    }

    for (u32 i = 0; i < 12; i++) {
        m_rollbacks[i].reset();
    }

    REPLACED(reset)();
}

void InputManager::calcPads(bool isPaused) {
    REPLACED(calcPads)(isPaused);

    for (u32 i = 0; i < 12; i++) {
        m_extraUserPads[i].calc();
    }

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

    if (auto *cameraManager = Graphics::CameraManager::Instance(); cameraManager &&
            cameraManager->isReady()) {
        if (auto *raceClient = SP::RaceClient::Instance()) {
            raceClient->calcWrite();
        }
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

void InputManager::calcRollbacks() {
    for (u32 i = 0; i < 12; i++) {
        m_rollbacks->calc(i);
    }
}

InputManager *InputManager::CreateInstance() {
    s_instance = new InputManager;
    assert(s_instance);

    s_instance->m_extraUserPads = new UserPad[12];
    s_instance->m_extraGhostPads = new GhostPad[12];
    s_instance->m_extraGhostProxies = new GhostPadProxy[12];
    for (u32 i = 0; i < 12; i++) {
        s_instance->m_extraGhostProxies[i].PadProxy::setPad(&s_instance->m_dummyPad, nullptr);
    }
    s_instance->m_rollbacks = new PadRollback[12];

    return s_instance;
}

InputManager *InputManager::Instance() {
    return s_instance;
}

} // namespace System
