#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <sp/cs/RoomManager.hh>

namespace UI {

class RandomMatchingPage : public Page {
public:
    RandomMatchingPage();
    ~RandomMatchingPage();

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void onRefocus() override;
    void afterCalc() override;

private:
    class Handler : public SP::RoomManager::Handler {
    public:
        Handler(RandomMatchingPage &page);
        ~Handler();

        void onSelect() override;
        void onError(const wchar_t *errorMessage) override;

    private:
        RandomMatchingPage &m_page;
    };

    CtrlMenuPageTitleText m_title;
    PageInputManager m_inputManager;
    Handler m_handler;
};

} // namespace UI
