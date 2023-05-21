#pragma once

#include "game/ui/LicenseControl.hh"
#include "game/ui/Page.hh"

namespace UI {

class LicenseDisplayPage : public Page {
public:
    LicenseDisplayPage();
    ~LicenseDisplayPage() override;
    PageId getReplacement() override;
    void onInit() override;
    REPLACE void onActivate() override;

private:
    u8 _044[0x34c - 0x044];
    LicenseControl m_licenseControl;
    u8 _4c0[0x634 - 0x4c0];
    MiiGroup m_miiGroup;
};
static_assert(sizeof(LicenseDisplayPage) == 0x6cc);

} // namespace UI
