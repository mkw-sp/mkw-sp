#pragma once

#include "game/ui/MessageWindowControl.hh"
#include "game/ui/Page.hh"

namespace UI {

class OptionExplanationPage : public Page {
public:
    void setTitleMessage(u32 messageId);
    void setWindowMessage(u32 messageId);
    u32 choice() const;

private:
    u8 _044[0xa30 - 0x044];
    u32 m_choice;
};
static_assert(sizeof(OptionExplanationPage) == 0xa34);

class OptionSelectPage : public Page {
public:
    void setTitleMessage(u32 messageId);
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr);
    void setButtonMessages(u32 index, u32 messageId, u32 confirmMessageId = 0);
    s32 choice() const;

private:
    u8 _044[0x404 - 0x044];
    MessageWindowControl m_messageWindow;
    u8 _578[0xee8 - 0x578];
    s32 m_choice;
    u8 _eec[0xef0 - 0xeec];
};
static_assert(sizeof(OptionSelectPage) == 0xef0);

} // namespace UI
