#include "MenuInputManager.hh"

namespace UI {

MenuInputManager::~MenuInputManager() {
    dt(-1);
}

PageInputManager::~PageInputManager() {
    dt(-1);
}

MultiControlInputManager::~MultiControlInputManager() {
    dt(-1);
}

u32 MultiControlInputManager::sequenceFlags(u32 localPlayerId) const {
    return m_players[localPlayerId].sequenceFlags();
}

bool MultiControlInputManager::isPointer(u32 localPlayerId) const {
    return m_players[localPlayerId].isPointer();
}

Vec2<f32> MultiControlInputManager::pointerPos(u32 localPlayerId) const {
    return m_players[localPlayerId].pointerPos();
}

bool MultiControlInputManager::isPerControl(u32 localPlayerId) const {
    return m_players[localPlayerId].isPerControl();
}

u32 MultiControlInputManager::Player::sequenceFlags() const {
    return m_sequenceFlags;
}

bool MultiControlInputManager::Player::isPointer() const {
    return m_isPointer;
}

Vec2<f32> MultiControlInputManager::Player::pointerPos() const {
    return m_pointerPos;
}

bool MultiControlInputManager::Player::isPerControl() const {
    return m_isPerControl;
}

} // namespace UI
