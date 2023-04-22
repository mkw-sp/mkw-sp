#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class LicenseControl : public LayoutUIControl {
public:
    static REPLACE void Refresh(void *r3, LicenseControl *control, u32 licenseId,
            MiiGroup *miiGroup, u32 index);
};

} // namespace UI
