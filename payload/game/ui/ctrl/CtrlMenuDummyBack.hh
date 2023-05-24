#pragma once

#include "game/ui/Button.hh"

namespace UI {

class CtrlMenuDummyBack : public LayoutUIControl {
public:
    CtrlMenuDummyBack();
    ~CtrlMenuDummyBack() override;

protected:
    void calcSelf() override;
    void vf_28() override;
    const char *getTypeName() override;

public:
    void load(const char *dir, const char *file, const char *variant, u32 playerFlags);

private:
    u8 _174[0x178 - 0x174];
};
static_assert(sizeof(CtrlMenuDummyBack) == 0x178);

} // namespace UI
