#pragma once

#include "game/ui/Page.hh"

namespace UI {

class MessagePage : public Page {
public:
    virtual void reset();
    virtual void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) = 0;
    virtual void vf_64() = 0;

private:
    u8 _044[0x1a5 - 0x044];
};

class MenuMessagePage : public MessagePage {
public:
    void reset() override;
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) override;

    void setTitleMessage(u32 messageId, MessageInfo *info = nullptr);

private:
    u8 _1a5[0x608 - 0x1a5];
};
static_assert(sizeof(MenuMessagePage) == 0x608);

} // namespace UI
