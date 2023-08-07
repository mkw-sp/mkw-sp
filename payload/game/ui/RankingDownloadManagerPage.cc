#include "RankingDownloadManagerPage.hh"

#include "game/host_system/SystemManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/AwaitPage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/SectionManager.hh"

#include <egg/core/eggSystem.hh>
#include <sp/settings/RegionLineColor.hh>

extern "C" {
#include <vendor/nanopb/pb_decode.h>
}

#include <cstdio>

namespace UI {

RankingDownloadManagerPage::RankingDownloadManagerPage() = default;

RankingDownloadManagerPage::~RankingDownloadManagerPage() = default;

PageId RankingDownloadManagerPage::getReplacement() {
    return m_replacement;
}

void RankingDownloadManagerPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
}

void RankingDownloadManagerPage::onDeinit() {
    if (m_initialisedNHTTPLibrary) {
        NHTTPCleanupAsync(nullptr);
    }
}

void RankingDownloadManagerPage::onActivate() {
    if (m_area == RankingPage::Area::Friend) {
        transition(State::InDevelopment);
    } else {
        if (makeRequest()) {
            transition(State::Request);
        } else {
            transition(State::Response);
        }
    }
}

void RankingDownloadManagerPage::afterCalc() {
    if (m_state == State::Request) {
        if (hasRequestTimedOut()) {
            NHTTPCancelRequestAsync(m_requestId);
        }
    }
}

void RankingDownloadManagerPage::onRefocus() {
    transition(resolve());
}

void RankingDownloadManagerPage::setCourse(Registry::Course course) {
    m_course = course;
}

void RankingDownloadManagerPage::setArea(RankingPage::Area area) {
    m_area = area;
}

RankingDownloadManagerPage::State RankingDownloadManagerPage::resolve() {
    switch (m_state) {
    case State::InDevelopment:
        return State::Finished;
    case State::Request:
        return State::Response;
    case State::Response:
        return State::Finished;
    case State::Finished:
        break;
    }

    return m_state;
}

void RankingDownloadManagerPage::transition(State state) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuMessagePage = section->page<PageId::MenuMessage>();
    auto *spinnerAwaitPage = section->page<PageId::SpinnerAwait>();

    switch (state) {
    case State::InDevelopment:
        menuMessagePage->reset();
        menuMessagePage->setWindowMessage(10430);
        push(PageId::MenuMessage, Anim::Next);
        break;
    case State::Request:
        spinnerAwaitPage->reset();
        spinnerAwaitPage->setWindowMessage(6114);
        push(PageId::SpinnerAwait, Anim::None);
        break;
    case State::Response:
        menuMessagePage->reset();
        switch (m_responseStatus) {
        case ResponseStatus::Ok:
            menuMessagePage->setWindowMessage(10430);
            break;
        case ResponseStatus::RequestError:
            menuMessagePage->setWindowMessage(10448);
            break;
        case ResponseStatus::ResponseError:
            menuMessagePage->setWindowMessage(10449);
            break;
        }
        push(PageId::MenuMessage, Anim::Next);
        break;
    case State::Finished:
        m_replacement = PageId::Ranking;
        startReplace(Anim::Next, 0.0f);
        break;
    }

    m_state = state;
}

bool RankingDownloadManagerPage::makeRequest() {
    if (!m_initialisedNHTTPLibrary) {
        if (NHTTPStartup(NHTTPAlloc, NHTTPFree, s_nhttpThreadPriority) != NHTTP_ERROR_NONE) {
            SP_LOG("Failed to start the 'NHTTP' library!");
            m_responseStatus = ResponseStatus::RequestError;
            return false;
        }
        m_initialisedNHTTPLibrary = true;
    }

    char url[64];
    snprintf(url, sizeof(url), "http://mkw-sp.com/api/GetTopTenRankings?region=%d&course=%d",
            getRegionParameterValue(), static_cast<s32>(m_course));

    NHTTPRequestHandle requestHandle = NHTTPCreateRequest(url, NHTTP_REQUEST_METHOD_GET,
            m_responseBuffer.data(), m_responseBuffer.size(), RequestCallback, this);
    assert(requestHandle);
    int requestId = NHTTPSendRequestAsync(requestHandle);
    assert(requestId >= 0);

    m_requestHandle = requestHandle;
    m_requestId = requestId;
    m_requestTimeout = OSGetTime() + OSSecondsToTicks(s_requestTimeoutSeconds);

    return true;
}

s32 RankingDownloadManagerPage::getRegionParameterValue() {
    assert(m_area != RankingPage::Area::Friend);

    if (m_area == RankingPage::Area::Regional) {
        auto *saveManager = System::SaveManager::Instance();
        SP::ClientSettings::RegionLineColor regionLineColorSetting =
                saveManager->getSetting<SP::ClientSettings::Setting::RegionLineColor>();
        if (regionLineColorSetting == SP::ClientSettings::RegionLineColor::Default) {
            return static_cast<s32>(System::SystemManager::Instance()->matchingArea());
        } else {
            return static_cast<s32>(regionLineColorSetting);
        }
    } else /* RankingPage::Area::Worldwide */ {
        return static_cast<s32>(SP::ClientSettings::RegionLineColor::Default);
    }
}

bool RankingDownloadManagerPage::hasRequestTimedOut() {
    return OSGetTime() >= m_requestTimeout;
}

void RankingDownloadManagerPage::requestCallback(NHTTPError error,
        NHTTPResponseHandle responseHandle) {
    if (error == NHTTP_ERROR_NONE) {
        m_responseStatus = processResponse(responseHandle);
    } else {
        m_responseStatus = ResponseStatus::RequestError;
    }
    NHTTPDestroyResponse(responseHandle);

    SectionManager::Instance()->currentSection()->page<PageId::SpinnerAwait>()->pop();
}

RankingDownloadManagerPage::ResponseStatus RankingDownloadManagerPage::processResponse(
        NHTTPResponseHandle responseHandle) {
    int statusCode = NHTTPGetResultCode(responseHandle);
    if (statusCode != NHTTP_STATUS_CODE_OK) {
        if (statusCode >= NHTTP_STATUS_CODE_BAD_REQUEST &&
                statusCode < NHTTP_STATUS_CODE_INTERNAL_SERVER_ERROR) {
            return ResponseStatus::RequestError;
        } else {
            return ResponseStatus::ResponseError;
        }
    }

    char *responseBody;
    int responseBodyLength = NHTTPGetBodyAll(responseHandle, &responseBody);
    if (responseBodyLength < 0) {
        return ResponseStatus::ResponseError;
    }

    pb_istream_t stream =
            pb_istream_from_buffer(reinterpret_cast<u8 *>(responseBody), responseBodyLength);
    if (!pb_decode(&stream, RankingResponse_fields, &m_rankingResponse)) {
        return ResponseStatus::ResponseError;
    }

    return ResponseStatus::Ok;
}

void *RankingDownloadManagerPage::NHTTPAlloc(u32 size, int align) {
    return EGG::TSystem::Instance().eggRootSystem()->alloc(size, align);
}

void RankingDownloadManagerPage::NHTTPFree(void *block) {
    EGG::TSystem::Instance().eggRootSystem()->free(block);
}

void RankingDownloadManagerPage::RequestCallback(NHTTPError error,
        NHTTPResponseHandle responseHandle, void *arg) {
    auto *rankingDownloadManagerPage = reinterpret_cast<RankingDownloadManagerPage *>(arg);
    rankingDownloadManagerPage->requestCallback(error, responseHandle);
}

} // namespace UI
