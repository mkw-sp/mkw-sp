#include "RankingPage.hh"

extern "C" {
#include "game/system/GhostFile.h"
}
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

void RankingPage::handleGhostDownload(f32 delay, u32 ghostType, u32 licenseId, s32 miiIndex) {
    m_ghostType = ghostType;

    switch (ghostType) {
    case GHOST_TYPE_PERSONAL_BEST:
        REPLACED(handleGhostDownload)(delay, ghostType, licenseId, miiIndex);
        break;
    default:
        m_replacement = PageId::SPRankingGhostDownload;
        startReplace(Anim::Next, delay);
        break;
    }
}

void RankingPage::handleTopTenDownload(f32 delay) {
    m_replacement = PageId::SPRankingTopTenDownload;
    startReplace(Anim::Next, delay);
}

UpDownControl &RankingPage::courseControl() {
    return m_courseControl;
}

RankingPage::Area RankingPage::area() const {
    return m_area;
}

Registry::Course RankingPage::course() const {
    return m_course;
}

u32 RankingPage::ghostType() const {
    return m_ghostType;
}

} // namespace UI
