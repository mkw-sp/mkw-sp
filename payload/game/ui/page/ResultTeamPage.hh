#pragma once

#include "game/ui/page/ResultPage.hh"

namespace UI {

class ResultTeamPage : public ResultPage {
public:
    ResultTeamPage();
    ~ResultTeamPage() override;
    void onInit() override;
    void afterCalc() override;
    TypeInfo *getTypeInfo() const override;
    bool isBusy() override;

    REPLACE static s32 GetScoreDiff();
};

} // namespace UI
