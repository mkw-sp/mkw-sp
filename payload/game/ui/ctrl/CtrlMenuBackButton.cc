#include "CtrlMenuBackButton.hh"

namespace UI {

CtrlMenuBackButton::CtrlMenuBackButton() : m_messageId(2001),
        m_fadeoutState(FadeoutState::Disabled), m_fadeoutOpacity(0.0f) {}

CtrlMenuBackButton::~CtrlMenuBackButton() {}

} // namespace UI
