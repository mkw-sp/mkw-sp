#pragma once

#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

class GhostWatchRaceMenuPage : public RaceMenuPage {
public:
    GhostWatchRaceMenuPage();
    ~GhostWatchRaceMenuPage() override;
    void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
    REPLACE bool getOptionClosable() override;
};

static_assert(sizeof(GhostWatchRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
