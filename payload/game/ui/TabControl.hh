#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class TabControl {
public:
    class OptionButton : public LayoutUIControl {
    public:
        OptionButton();
        ~OptionButton() override;
        void init() override;
        void calc() override;
        void vf_28() override;
        const char *getTypeName() override;

        void load(u32 i, const char *dir, const char *file, const char *variant, bool r8,
                bool pointerOnly);
        void setPlayerFlags(u32 playerFlags);
        void setChosen(bool chosen);
        void select(u32 localPlayerId);
        void onSelect(u32 localPlayerId);

        ControlInputManager m_inputManager;
        u8 _1f8[0x234 - 0x1f8];
        u32 m_index;
        u8 _238[0x240 - 0x238];
    };
    static_assert(sizeof(OptionButton) == 0x240);
};

} // namespace UI
