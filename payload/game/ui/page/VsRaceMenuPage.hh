#pragma once

#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

class VsRaceMenuPage : public RaceMenuPage {
public:
    VsRaceMenuPage();
    ~VsRaceMenuPage() override;
    void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
    REPLACE const char *getResFileName() override;
};

static_assert(sizeof(VsRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
