#pragma once

#include "RaceMenuPage.hh"

namespace UI {

class BtRaceMenuPage : public RaceMenuPage {
public:
    BtRaceMenuPage();
    ~BtRaceMenuPage() override;
    virtual void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
    REPLACE const char *getResFileName() override;
};

static_assert(sizeof(BtRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
