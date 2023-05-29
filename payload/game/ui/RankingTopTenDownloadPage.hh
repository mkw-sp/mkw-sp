#pragma once

#include "game/ui/ConfirmPage.hh"

namespace UI {

class RankingTopTenDownloadPage : public Page {
public:
    RankingTopTenDownloadPage();
    ~RankingTopTenDownloadPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onTopTenButtonSelect(ConfirmPage *confirmPage, f32 delay);

    template <typename T>
    using H = typename T::template Handler<RankingTopTenDownloadPage>;

    MenuInputManager m_menuInputManager;
    H<ConfirmPage> m_onTopTenButtonSelect{this, &RankingTopTenDownloadPage::onTopTenButtonSelect};
};

} // namespace UI
