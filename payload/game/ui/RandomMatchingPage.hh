#pragma once

#include "payload/game/ui/Page.hh"
#include "payload/game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class RandomMatchingPage : public Page {
public:
    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void onRefocus() override;
    void afterCalc() override;

private:
    CtrlMenuPageTitleText m_title;
    PageInputManager m_inputManager;
};

}
