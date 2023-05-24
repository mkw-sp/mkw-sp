#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class AwardDemoResultItem : public LayoutUIControl {
public:
    AwardDemoResultItem();
    virtual ~AwardDemoResultItem();
    void initSelf() override;
    void vf_28() override;
    const char *getTypeName() override;

    void load(u32 positionId, bool isWin, bool r6);
    void refresh(u32 playerId, bool isTeams, bool isMultiPlayer);
    void refresh(u32 playerId, bool isMultiPlayer, u32 soloRank);
};

} // namespace UI
