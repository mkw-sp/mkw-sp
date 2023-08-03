#pragma once

#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

class ToRaceMenuPage : public RaceMenuPage {
public:
    ToRaceMenuPage();
    ~ToRaceMenuPage() override;
    void dt(s32 type) override;

    REPLACE const ButtonId *getButtons() override;
    REPLACE const char *getResFileName() override;
};

static_assert(sizeof(ToRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
