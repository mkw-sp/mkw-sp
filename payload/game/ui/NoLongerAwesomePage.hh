#pragma once

#include "game/ui/NoLongerAwesomeSubPage.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"

namespace UI {

class NoLongerAwesomePage : public Page {
public:
    NoLongerAwesomePage();
    ~NoLongerAwesomePage() override;

    void onInit() override;

private:
    void onButtonFront(PushButton *button, u32 localPlayerId);
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<NoLongerAwesomePage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuBackButton m_backButton;
    std::array<PushButton, 3> m_buttons;

    H<PushButton> m_onButtonFront{this, &NoLongerAwesomePage::onButtonFront};
    H<MultiControlInputManager> m_onBack{this, &NoLongerAwesomePage::onBack};
    H<PushButton> m_onBackButtonFront{this, &NoLongerAwesomePage::onBackButtonFront};
};

} // namespace UI
