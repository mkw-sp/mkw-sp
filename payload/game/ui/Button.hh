#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class PushButton : public LayoutUIControl {
public:
    PushButton();
    void dt(s32 type) override;
    void init() override;
    void calc() override;
    void vf_28() override;
    void vf_2c() override;
    virtual void vf_3c();
    virtual void vf_40();
    virtual void vf_44();
    virtual void vf_48();

    void load(const char *dir, const char *file, const char *variant, u32 playerFlags, bool r8,
            bool pointerOnly);
    void selectDefault(u32 localPlayerId);

private:
    u8 _174[0x240 - 0x174];
    s32 m_index;
    u8 _244[0x254 - 0x244];
};
static_assert(sizeof(PushButton) == 0x254);

} // namespace UI
