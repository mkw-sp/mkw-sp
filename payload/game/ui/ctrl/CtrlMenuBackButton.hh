#pragma once

#include "game/ui/Button.hh"

namespace UI {

class CtrlMenuBackButton : public PushButton {
public:
    CtrlMenuBackButton();
    ~CtrlMenuBackButton() override;

protected:
    void initSelf() override;
    void calcSelf() override;
    void vf_20() override;
    void vf_28() override;
    void vf_2c() override;

private:
    enum class FadeoutState {
        Disabled,
        Enabled,
    };

    u32 m_messageId = 2001;
    FadeoutState m_fadeoutState = FadeoutState::Disabled;
    f32 m_fadeoutOpacity = 0.0f;
    u8 _260[0x264 - 0x260];
};
static_assert(sizeof(CtrlMenuBackButton) == 0x264);

} // namespace UI
