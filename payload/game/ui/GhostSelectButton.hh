#pragma once

#include "TabControl.hh"

namespace UI {

class GhostSelectButton : public TabControl::OptionButton {
public:
    GhostSelectButton();
    ~GhostSelectButton();

    void load(u32 i);
    void refresh(u32 listIndex);

    bool isChosen() const { return m_chosen; }

private:
    void onSelect(u32 localPlayerId, u32 r5);
    void onFront(u32 localPlayerId, u32 r5);

    class TimeAttackGhostListPage *getGhostListPage();

    template <typename T>
    using H = typename T::Handler<GhostSelectButton>;

    MiiGroup m_miiGroup;
    H<ControlInputManager> m_onSelect{ this, &GhostSelectButton::onSelect };
    H<ControlInputManager> m_onFront{ this, &GhostSelectButton::onFront };
    bool m_chosen = false;
};

}  // namespace UI
