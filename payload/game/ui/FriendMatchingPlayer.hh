#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class FriendMatchingPlayer final : public LayoutUIControl {
public:
    struct Callback {
        u8 _00[0x14 - 0x00];
    };
    static_assert(sizeof(Callback) == 0x14);

    FriendMatchingPlayer();
    ~FriendMatchingPlayer() override;
    void initSelf() override;
    void calcSelf() override;
    void vf_28() override;
    const char *getTypeName() override;

    void load(MiiGroup *miiGroup, u8 i, u8 j);
    void show(u32 pos, u32 count);
    void move(u32 pos, u32 count);
    void hide();
    Callback &callback();

private:
    Callback m_callback;
    u8 _188[0x1a0 - 0x188];
};
static_assert(sizeof(FriendMatchingPlayer) == 0x1a0);

} // namespace UI
