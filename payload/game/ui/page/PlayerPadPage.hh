#pragma once

#include "game/ui/Page.hh"

namespace UI {

class PlayerPadPage : public Page {
public:
    PlayerPadPage();
    ~PlayerPadPage() override;
    void dt(s32 type) override;

    REPLACE PageId getReplacement() override;

private:
    u8 _044[0x674 - 0x44];
};

static_assert(sizeof(PlayerPadPage) == 0x674);

} // namespace UI