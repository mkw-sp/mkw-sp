#pragma once

#include "game/ui/ctrl/CtrlRaceResultTeam.hh"
#include "game/ui/ctrl/CtrlRaceResultTeamPoint.hh"
#include "game/ui/page/ResultPage.hh"

#include <array>

namespace UI {

class ResultTeamTotalPage : public ResultPage {
public:
    ResultTeamTotalPage();
    ~ResultTeamTotalPage() override;
    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void beforeCalc() override;
    bool isBusy() override;
    void vf_68() override;

private:
    CtrlRaceResultTeam m_controls[12];
    CtrlRaceResultTeamPoint m_pointControls[6];
    std::array<u32, 12> m_playerScores;
    std::array<u32, 6> m_teamScores;
    u32 m_frame;
    bool m_isBusy;
};

} // namespace UI
