#pragma once

#include "game/ui/AwardDemoCongratulations.hh"
#include "game/ui/AwardDemoResultItem.hh"
#include "game/ui/Page.hh"

namespace UI {

class AwardPage : public Page {
public:
    AwardPage();
    ~AwardPage() override;

    void onInit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void afterCalc() override;
    TypeInfo *getTypeInfo() const override;

private:
    void onFront(u32 localPlayerId);

    REPLACE void initConfig();
    REPLACE void initType();
    void REPLACED(initCongratulations)();
    REPLACE void initCongratulations();
    REPLACE void initItems();
    void initTeams();
    REPLACE void initCup();

    template <typename T>
    using H = typename T::template Handler<AwardPage>;

    PageInputManager m_inputManager;
    H<PageInputManager> m_onFront{this, &AwardPage::onFront};
    u8 _0054[0x005c - 0x0054]; // The first 4 bytes are unused
    LayoutUIControl m_cupDisplay;
    u8 _0314[0x0488 - 0x0314]; // Unused
    AwardDemoCongratulations m_congratulations;
    AwardDemoResultItem m_items[12];
    u8 _176c[0x176d - 0x176c];
    u8 m_bestRank;
    u8 m_localPlayerCount;
    u8 m_playerCount;
    LayoutUIControl m_teams[6]; // Added
};
static_assert(sizeof(AwardPage) == 0x1770 + sizeof(LayoutUIControl) * 6);

} // namespace UI
