#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

class MissionInstructionPage : public MenuPage {
public:
    u32 levelId() const;

private:
    u8 _430[0x83c - 0x430];
    u32 m_levelId;
    u8 _840[0xd60 - 0x840];
};
static_assert(sizeof(MissionInstructionPage) == 0xd60);

} // namespace UI
