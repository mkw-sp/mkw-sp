#pragma once

#include "game/ui/page/MenuPage.hh"

#include "game/system/RaceConfig.hh"

namespace UI {

class DriftSelectPage : public MenuPage {
public:
    DriftSelectPage();
    ~DriftSelectPage() override;

    void REPLACED(onActivate)();
    REPLACE void onActivate() override;

    void setReplacementSection(SectionId section) {
        m_replacementSection = section;
    }

    REPLACE void onButtonFront(PushButton *button, u32 localPlayerId);

private:
    // Reimplemented as original game uses undocumented GlobalContext state
    bool selectRandomCourse(System::RaceConfig::Scenario &menuScenario);

    u8 _430[0x6c8 - 0x430];
    SectionId m_replacementSection;
};
static_assert(sizeof(DriftSelectPage) == 0x6cc);

} // namespace UI
