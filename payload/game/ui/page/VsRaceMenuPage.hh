#pragma once

#include "RaceMenuPage.hh"

namespace UI {

class VsRaceMenuPage : public RaceMenuPage {
public:
    VsRaceMenuPage();
    ~VsRaceMenuPage() override;
    virtual void dt(s32 type) override;

    REPLACE s32 getButtonCount() override;
    REPLACE const ButtonId *getButtons() override;
    REPLACE const char *getResFileName() override;
};

static_assert(sizeof(VsRaceMenuPage) == sizeof(RaceMenuPage));

} // namespace UI
