#pragma once

#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

class TaRaceMenuPage : public RaceMenuPage {
public:
    TaRaceMenuPage();
    ~TaRaceMenuPage() override;
    virtual void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
};

static_assert(sizeof(TaRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
