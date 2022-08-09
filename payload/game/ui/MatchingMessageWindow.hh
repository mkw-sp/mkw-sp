#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class MatchingMessageWindow : public LayoutUIControl {
public:
    MatchingMessageWindow();
    ~MatchingMessageWindow() override;
    void initSelf() override;
    void calcSelf() override;
    void vf_28() override;
    void vf_2c() override;

    void load(const char *variant);
    void show(u32 messageId, MessageInfo *info = nullptr);
    void hide();
};
static_assert(sizeof(MatchingMessageWindow) == 0x174);

} // namespace UI
