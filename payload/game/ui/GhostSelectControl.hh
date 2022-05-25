#pragma once

#include <array>
#include "game/ui/GhostSelectButton.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class GhostSelectControl : public LayoutUIControl {
public:
    GhostSelectControl();
    ~GhostSelectControl();

    void load();

    bool isHidden();
    bool isShown();

    void hide();
    void slideInFromLeft();
    void slideInFromRight();

    void show();
    void slideOutToLeft();
    void slideOutToRight();

    void calcSelf() override;

    std::array<GhostSelectButton, 6> buttons;

private:
    class TimeAttackGhostListPage *getGhostListPage();

    u32 getSlideAnimation();
    void setSlideAnimation(u32 animationId);
    bool hasSlidIn();
    void onHide();
    void onShow();
};

}  // namespace UI