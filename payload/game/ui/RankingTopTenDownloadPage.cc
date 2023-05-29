#include "RankingTopTenDownloadPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

RankingTopTenDownloadPage::RankingTopTenDownloadPage() = default;

RankingTopTenDownloadPage::~RankingTopTenDownloadPage() = default;

PageId RankingTopTenDownloadPage::getReplacement() {
    return PageId::Ranking;
}

void RankingTopTenDownloadPage::onInit() {
    m_menuInputManager.init(0x1, false);
    setInputManager(&m_menuInputManager);
    initChildren(0);
}

void RankingTopTenDownloadPage::onActivate() {
    ConfirmPage *confirmPage =
            SectionManager::Instance()->currentSection()->page<PageId::Confirm>();

    confirmPage->reset();
    confirmPage->setWindowMessage(10430);
    confirmPage->m_confirmHandler = &m_onTopTenButtonSelect;
    confirmPage->m_cancelHandler = &m_onTopTenButtonSelect;
    confirmPage->setAnim(Anim::Next);
    push(PageId::Confirm, Anim::None);
}

void RankingTopTenDownloadPage::onTopTenButtonSelect(ConfirmPage *confirmPage, f32 delay) {
    confirmPage->setAnim(Anim::Prev);
    startReplace(Anim::Prev, delay);
}

} // namespace UI
