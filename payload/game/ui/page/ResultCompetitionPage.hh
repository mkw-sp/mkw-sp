#pragma once

#include "ResultPlayerPage.hh"

namespace UI {

class ResultCompetitionPage : public ResultPlayerPage {
public:
    ResultCompetitionPage();
    ~ResultCompetitionPage() override;
    void dt(s32 type) override;

    REPLACE PageId getReplacement() override;
};

static_assert(sizeof(ResultCompetitionPage) == sizeof(ResultPlayerPage));

} // namespace UI
