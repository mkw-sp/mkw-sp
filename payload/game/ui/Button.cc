#include "Button.hh"

namespace UI {

PushButton::~PushButton() = default;

void PushButton::setPointerOnly(bool pointerOnly) {
    m_inputManager.m_pointerOnly = pointerOnly;
}

} // namespace UI
