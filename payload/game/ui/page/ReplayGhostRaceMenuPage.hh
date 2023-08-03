#pragma once

#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

class ReplayGhostRaceMenuPage : public RaceMenuPage {
public:
    ReplayGhostRaceMenuPage();
    ~ReplayGhostRaceMenuPage() override;
    void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
};

static_assert(sizeof(ReplayGhostRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
