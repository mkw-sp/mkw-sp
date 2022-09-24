#pragma once

#include "game/ui/Button.hh"

namespace UI {

class CtrlMenuCharacterSelect : public LayoutUIControl {
    // ...

private:
    class ButtonDriver : public PushButton {
    public:
        ButtonDriver();
        ~ButtonDriver() override;
        void REPLACED(initSelf)();
        REPLACE void initSelf() override;
        // ...

    private:
        u8 _254[0x260 - 0x254];
    };
    static_assert(sizeof(ButtonDriver) == 0x260);
};

} // namespace UI
