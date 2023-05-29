#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class BlackBackControl : public LayoutUIControl {
public:
    BlackBackControl();
    ~BlackBackControl() override;

    void vf_20() override;
    void vf_28() override;
    const char *getTypeName() override;

    void load(const char *dir, const char *file, const char *variant);
};
static_assert(sizeof(BlackBackControl) == 0x174);

} // namespace UI
