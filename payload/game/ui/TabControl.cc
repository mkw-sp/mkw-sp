#include "TabControl.hh"

namespace UI {

TabControl::OptionButton::~OptionButton() {
    dt(-1);
}

void TabControl::OptionButton::select(u32 localPlayerId) {
    m_inputManager.m_parent->select(localPlayerId, &m_inputManager, -1);
}

} // namespace UI
