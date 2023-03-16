#pragma once

#include "game/ui/Page.hh"

namespace UI {

class TitlePage : public Page {
private:
    void REPLACED(onInit)();
    void REPLACED(onActivate)();
    REPLACE void onInit() override;
    REPLACE void onActivate() override;

    u8 _44[0x53 - 0x44];
    bool m_geckoWarningShown; // Was padding
    u8 _54[0x348 - 0x54];
    bool m_useAlternativeBackgrounds;
    u8 _349[0x34c - 0x349];
    u32 m_alternativeBackground;
};

static_assert(sizeof(TitlePage) == 0x350);

} // namespace UI