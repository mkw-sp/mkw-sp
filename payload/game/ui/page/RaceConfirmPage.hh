#pragma once

#include "game/ui/Page.hh"

namespace UI {

class RaceConfirmPage : public Page {
public:
    bool hasConfirmed() const;

private:
    u8 _044[0x6c4 - 0x044];
    bool m_hasConfirmed;
    u8 _6c5[0x6cc - 0x6c5];
};
static_assert(sizeof(RaceConfirmPage) == 0x6cc);

} // namespace UI
