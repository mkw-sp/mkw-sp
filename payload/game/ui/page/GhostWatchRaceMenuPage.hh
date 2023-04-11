#pragma once

#include "RaceMenuPage.hh"

namespace UI {

class GhostWatchRaceMenuPage : public RaceMenuPage {
public:
    GhostWatchRaceMenuPage();
    ~GhostWatchRaceMenuPage() override;
    virtual void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
    REPLACE bool getOptionClosable() override;
};

static_assert(sizeof(GhostWatchRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
