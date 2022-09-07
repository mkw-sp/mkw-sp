#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

class DriftSelectPage : public MenuPage {
public:
    void setReplacementSection(SectionId section) { m_replacementSection = section; }
private:
    u8 _430[0x6c8 - 0x430];
    SectionId m_replacementSection;
};
static_assert(sizeof(DriftSelectPage) == 0x6cc);

} // namespace UI
