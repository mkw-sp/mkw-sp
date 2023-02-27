#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/UIControl.hh"

#include <memory>

namespace UI {

class RadioButtonControl : public LayoutUIControl {
public:
    class OptionButton : public LayoutUIControl {
    public:
        OptionButton();
        ~OptionButton() override;
        void init() override;
        void calc() override;
        void vf_28() override;
        void vf_2c() override;

    private:
        u8 _174[0x17c - 0x174];
    };
    static_assert(sizeof(OptionButton) == 0x17c);

    RadioButtonControl();
    ~RadioButtonControl() override;
    void init() override;
    void calc() override;
    void vf_28() override;
    void vf_2c() override;
    virtual void vf_3c();
    virtual void vf_40();

private:
    u8 _174[0x210 - 0x174];
    ControlInputManager m_inputManager;
    std::unique_ptr<OptionButton[]> m_buttons;
};
static_assert(sizeof(RadioButtonControl) == 0x298);

} // namespace UI
