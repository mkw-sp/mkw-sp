#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class CourseSelectPage : public Page {
public:
    CourseSelectPage();
    CourseSelectPage(const CourseSelectPage &) = delete;
    CourseSelectPage(CourseSelectPage &&) = delete;
    ~CourseSelectPage() override;

    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<CourseSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &CourseSelectPage::onBack };
    H<PushButton> m_onBackButtonFront{ this, &CourseSelectPage::onBackButtonFront };
    PageId m_replacement;
};

} // namespace UI
