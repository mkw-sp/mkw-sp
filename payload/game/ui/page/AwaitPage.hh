#pragma once

#include "game/ui/Page.hh"

namespace UI {

class AwaitPage : public Page {
public:
    virtual void reset() = 0;
    virtual void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) = 0;

    void pop();

private:
    u8 _44[0x55 - 0x44];
};

class MenuAwaitPage : public AwaitPage {
public:
    void reset() override;
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) override;

    void setTitleMessage(u32 messageId, MessageInfo *info = nullptr);

private:
    u8 _055[0x348 - 0x055];
};
static_assert(sizeof(MenuAwaitPage) == 0x348);

} // namespace UI
