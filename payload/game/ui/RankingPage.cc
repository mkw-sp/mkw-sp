#include "RankingPage.hh"

#include "game/ui/CourseSelectPage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

RankingPage::RankingPage() {
    construct();
}

void RankingPage::onInit() {
    Section *section = SectionManager::Instance()->currentSection();
    section->loadFriendListManager();

    auto *courseSelectPage = section->page<PageId::CourseSelect>();
    courseSelectPage->filter(SP::Track::Mode::Race);

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
    m_replacement = PageId::RankingTopTenDownload;
    startReplace(Anim::Next, delay);
}

UpDownControl &RankingPage::courseControl() {
    return m_courseControl;
}

void RankingPage::pop(ConfirmPage * /* confirmPage */, f32 delay) {
    changeSection(SectionId::ServicePackChannel, Anim::Prev, delay);
}

} // namespace UI
