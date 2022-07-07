#include "Option.hh"

namespace UI {

u32 OptionExplanationPage::choice() const {
    return m_choice;
}

void OptionSelectPage::setWindowMessage(u32 messageId, MessageInfo *info) {
    m_messageWindow.setMessageAll(messageId, info);
}

s32 OptionSelectPage::choice() const {
    return m_choice;
}

} // namespace UI
