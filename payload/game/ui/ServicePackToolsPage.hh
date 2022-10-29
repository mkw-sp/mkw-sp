#pragma once

#include "game/ui/MessagePage.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class ServicePackToolsPage : public Page {
public:
    ServicePackToolsPage();
    ServicePackToolsPage(const ServicePackToolsPage &) = delete;
    ServicePackToolsPage(ServicePackToolsPage &&) = delete;
    ~ServicePackToolsPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onStorageBenchmarkButtonFront(PushButton *button, u32 localPlayerId);
    void onThumbnailsButtonFront(PushButton *button, u32 localPlayerId);
    void onServerModeButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onThumbnailsNoCoursePop(MessagePage *messagePage);

    template <typename T>
    using H = typename T::Handler<ServicePackToolsPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_storageBenchmarkButton;
    PushButton m_thumbnailsButton;
    PushButton m_serverModeButton;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &ServicePackToolsPage::onBack };
    H<PushButton> m_onStorageBenchmarkButtonFront{ this,
            &ServicePackToolsPage::onStorageBenchmarkButtonFront };
    H<PushButton> m_onServerModeButtonFront{ this, &ServicePackToolsPage::onServerModeButtonFront };
    H<PushButton> m_onThumbnailsButtonFront{ this, &ServicePackToolsPage::onThumbnailsButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &ServicePackToolsPage::onBackButtonFront };
    H<MessagePage> m_onThumbnailsNoCoursePop{ this, &ServicePackToolsPage::onThumbnailsNoCoursePop };
    PageId m_replacement;
};

} // namespace UI
