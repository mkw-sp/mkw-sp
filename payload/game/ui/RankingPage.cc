#include "RankingPage.hh"

#include "game/ui/CourseSelectPage.hh"
#include "game/ui/RankingDownloadManagerPage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

RankingPage::RankingPage() {
    construct();
}

void RankingPage::onInit() {
    Section *section = SectionManager::Instance()->currentSection();
    section->loadFriendListManager();

    auto *courseSelectPage = section->page<PageId::CourseSelect>();
    courseSelectPage->filter({true, false});

    REPLACED(onInit)();
}

void RankingPage::onCourseChange(UpDownControl *upDownControl, u32 localPlayerId, s32 index) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *courseSelectPage = section->page<PageId::CourseSelect>();
    courseSelectPage->refreshSelection(index);

    REPLACED(onCourseChange)(upDownControl, localPlayerId, index);
}

void RankingPage::onBack(PushButton *pushButton, u32 /* localPlayerId */) {
    changeSection(SectionId::ServicePackChannel, Anim::Prev, pushButton->getDelay());
}

void RankingPage::onBack() {
    changeSection(SectionId::ServicePackChannel, Anim::Prev, 0.0f);
}

void RankingPage::handleTopTenDownload(f32 delay) {
    auto *sectionManager = SectionManager::Instance();
    auto *rankingDownloadManagerPage =
            sectionManager->currentSection()->page<PageId::RankingDownloadManager>();

    rankingDownloadManagerPage->setCourse(
            sectionManager->globalContext()->GetCourseFromButtonIndex(m_courseControl.chosen()));
    rankingDownloadManagerPage->setArea(static_cast<Area>(m_areaControl.chosen()));

    m_replacement = PageId::RankingDownloadManager;
    startReplace(Anim::Next, delay);
}

UpDownControl &RankingPage::courseControl() {
    return m_courseControl;
}

void RankingPage::pop(ConfirmPage * /* confirmPage */, f32 delay) {
    changeSection(SectionId::ServicePackChannel, Anim::Prev, delay);
}

} // namespace UI
