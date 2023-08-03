#pragma once

#include "game/ui/Page.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class TimeAttackSplitsPage : public Page {
public:
    TimeAttackSplitsPage();
    ~TimeAttackSplitsPage() override;

    TimeAttackSplitsPage *REPLACED(ct)();
    REPLACE TimeAttackSplitsPage *ct();
    REPLACE PageId getReplacement() override;
    REPLACE void afterCalc() override;

    REPLACE void onFront();

private:
    u8 _044[0x04c - 0x044];
    bool m_isReady; // Modified
    u8 _050[0x370 - 0x04d];
    LayoutUIControl m_ghostMessage;
};

static_assert(sizeof(TimeAttackSplitsPage) == 0x4e4);

} // namespace UI
