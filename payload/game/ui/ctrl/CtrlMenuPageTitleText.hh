#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class CtrlMenuPageTitleText : public LayoutUIControl {
public:
    CtrlMenuPageTitleText();
    void dt(s32 type) override;

protected:
    void vf_20() override;
    void vf_28() override;
    void vf_2c() override;

public:
    void load(bool isOptions);
    void setMessage(s32 messageId, MessageInfo *info);
};
static_assert(sizeof(CtrlMenuPageTitleText) == 0x174);

} // namespace UI
