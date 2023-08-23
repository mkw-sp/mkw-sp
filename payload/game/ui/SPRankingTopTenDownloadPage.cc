#include "SPRankingTopTenDownloadPage.hh"

#include "game/ui/AwaitPage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/SectionManager.hh"

#include <cstdio>

namespace UI {

SPRankingTopTenDownloadPage::SPRankingTopTenDownloadPage() = default;

SPRankingTopTenDownloadPage::~SPRankingTopTenDownloadPage() = default;

void SPRankingTopTenDownloadPage::onActivate() {
    auto *section = SectionManager::Instance()->currentSection();

    m_state = State::Previous;
    switch (section->page<PageId::Ranking>()->area()) {
    case RankingPage::Area::Friend:
        transition(State::InDevelopment);
        break;
    case RankingPage::Area::Regional:
    case RankingPage::Area::Worldwide:
        char url[60];
        snprintf(url, sizeof(url), "http://mkw-sp.com/api/GetTopTenRankings?region=%d&course=%d",
                getRegionParameterValue(), getCourseParameterValue());
        if (makeRequest(url)) {
            transition(State::Request);
        } else {
            transition(State::RequestError);
        }
        break;
    }
}

void SPRankingTopTenDownloadPage::transition(State state) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuMessagePage = section->page<PageId::MenuMessage>();
    auto *spinnerAwaitPage = section->page<PageId::SpinnerAwait>();

    if (state == m_state) {
        return;
    }

    switch (state) {
    case State::Previous:
        m_replacement = PageId::Ranking;
        startReplace(Anim::Prev, 0.0f);
        break;
    case State::InDevelopment:
        menuMessagePage->reset();
        menuMessagePage->setWindowMessage(10430);
        push(PageId::MenuMessage, Anim::Next);
        break;
    case State::Request:
        spinnerAwaitPage->reset();
        spinnerAwaitPage->setWindowMessage(6114);
        push(PageId::SpinnerAwait, Anim::Next);
        break;
    case State::RequestError:
        menuMessagePage->reset();
        menuMessagePage->setWindowMessage(10448);
        push(PageId::MenuMessage, Anim::Next);
        break;
    case State::Response:
        menuMessagePage->reset();
        switch (responseStatus()) {
        case ResponseStatus::Ok:
            menuMessagePage->setWindowMessage(10430);
            break;
        case ResponseStatus::Error:
            menuMessagePage->setWindowMessage(10449);
            break;
        }
        push(PageId::MenuMessage, Anim::Next);
        break;
    }

    m_state = state;
}

} // namespace UI
