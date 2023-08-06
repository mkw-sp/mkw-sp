#pragma once

#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

class AfterTaRaceMenuPage : public RaceMenuPage {
public:
    AfterTaRaceMenuPage();
    ~AfterTaRaceMenuPage() override;
    void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
};

static_assert(sizeof(AfterTaRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
