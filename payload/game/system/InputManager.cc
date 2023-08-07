#include "InputManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

extern "C" {
#include <revolution/kpad.h>
}
#include <sp/SaveStateManager.hh>
#include <sp/cs/RaceClient.hh>

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

void WiiPad::process(RaceInputState &raceInputState, UIInputState &uiInputState) {
    REPLACED(process)(raceInputState, uiInputState);

    if (InputManager::Instance()->isMirror()) {
        uiInputState.stick.x *= -1.0f;
    }
}

void WiiPad::processClassic(void *r4, RaceInputState &raceInputState, UIInputState &uiInputState) {
    REPLACED(processClassic)(r4, raceInputState, uiInputState);

    processSimplified(raceInputState, raceInputState.rawButtons & KPAD_CL_TRIGGER_ZL);

    if (auto *saveStateManager = SP::SaveStateManager::Instance()) {
        saveStateManager->processInput(raceInputState.rawButtons & KPAD_CL_BUTTON_MINUS);
    }
}

void GCPad::process(RaceInputState &raceInputState, UIInputState &uiInputState) {
    REPLACED(process)(raceInputState, uiInputState);

    processSimplified(raceInputState, raceInputState.rawButtons & PAD_BUTTON_Y);

    if (InputManager::Instance()->isMirror()) {
        uiInputState.stick.x *= -1.0f;
    }
}

void GhostPad::process(RaceInputState &raceInputState, UIInputState &uiInputState) {
    REPLACED(process)(raceInputState, uiInputState);
    auto *rc = System::RaceConfig::Instance();
    // Flips the inputs of ghosts whenever the mode is mirror
    if (rc->raceScenario().mirror) {
        raceInputState.stick.x *= -1;
        raceInputState.SetTrick(raceInputState, raceInputState.trick);
    }
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

    u32 time = RaceManager::Instance()->time();
    if (auto frame = serverFrame(playerId)) {
        s32 delay = static_cast<s32>(time) - static_cast<s32>(frame->time);
        if (delay <= 0) {
            handleFutureFrame(*frame);
        } else {
            handlePastFrame(*frame);
        }
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (m_frames[i]->time == time - 1) {
                applyFrame(playerId, *m_frames[i]);
                break;
            }
        }
    }

    if (!m_frames.back() || m_frames.back()->time < time) {
        if (m_frames.full()) {
            m_frames.pop_front();
        }
        auto *proxy = System::InputManager::Instance()->userProxy(playerId);
        m_frames.push_back({time, proxy->currentRaceInputState()});
    }
}

std::optional<PadRollback::Frame> PadRollback::serverFrame(u32 playerId) const {
    auto serverFrame = SP::RaceClient::Instance()->frame();
    if (!serverFrame) {
        return {};
    }

    u32 time = serverFrame->time;
    const auto &player = serverFrame->players[playerId];
    RaceInputState inputState;
    inputState.accelerate = player.inputState.accelerate;
    inputState.brake = player.inputState.brake;
    inputState.item = false;
    inputState.drift = player.inputState.drift;
    inputState.brakeDrift = player.inputState.brakeDrift; // TODO check for 200cc
    System::RaceInputState::SetStickX(inputState, player.inputState.stickX);
    System::RaceInputState::SetStickY(inputState, player.inputState.stickY);
    System::RaceInputState::SetTrick(inputState, player.inputState.trick);
    return {{time, inputState}};
}

void PadRollback::handleFutureFrame(const Frame &frame) {
    u32 time = System::RaceManager::Instance()->time();
    while (m_frames.front() && m_frames.front()->time < time) {
        m_frames.pop_front();
    }
    if (!m_frames.full()) {
        m_frames.push_back(std::move(frame));
    }
}

void PadRollback::handlePastFrame(const Frame &frame) {
    while (m_frames.front() && m_frames.front()->time < frame.time) {
        m_frames.pop_front();
    }
    auto *rollbackFrame = m_frames.front();
    if (rollbackFrame && rollbackFrame->time == frame.time) {
        for (u32 i = 0; i < m_frames.count(); i++) {
            m_frames[i]->inputState = frame.inputState;
        }
    }
}

void PadRollback::applyFrame(u32 playerId, const Frame &frame) {
    auto *proxy = System::InputManager::Instance()->userProxy(playerId);
    proxy->setRaceInputState(frame.inputState);
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

const PADStatus &InputManager::padStatus(u32 i) const {
    return m_padStatus[i];
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

    for (u32 i = 0; i < 12; i++) {
        m_rollbacks[i].reset();
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
    REPLACED(calc)();

    for (u32 i = 0; i < 12; i++) {
        m_extraGhostProxies[i].calc(m_isPaused);
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
