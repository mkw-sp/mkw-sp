#include "SPRankingDownloadPage.hh"

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

namespace UI {

bool SPRankingDownloadPage::s_initialisedNHTTPLibrary = false;

SPRankingDownloadPage::SPRankingDownloadPage() = default;

SPRankingDownloadPage::~SPRankingDownloadPage() = default;

PageId SPRankingDownloadPage::getReplacement() {
    return m_replacement;
}

void SPRankingDownloadPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
}

void SPRankingDownloadPage::onDeinit() {
    if (s_initialisedNHTTPLibrary) {
        NHTTPCleanupAsync(nullptr);
        s_initialisedNHTTPLibrary = false;
    }
}

void SPRankingDownloadPage::afterCalc() {
    if (m_state == State::Request) {
        if (hasRequestTimedOut()) {
            NHTTPCancelRequestAsync(m_requestId);
        }
    }
}

void SPRankingDownloadPage::onRefocus() {
    transition(resolve());
}

bool SPRankingDownloadPage::makeRequest(const char *url) {
    if (!s_initialisedNHTTPLibrary) {
        if (NHTTPStartup(NHTTPAlloc, NHTTPFree, s_nhttpThreadPriority) != NHTTP_ERROR_NONE) {
            SP_LOG("Failed to start the 'NHTTP' library!");
            m_responseStatus = ResponseStatus::RequestError;
            return false;
        }
        s_initialisedNHTTPLibrary = true;
    }

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

s32 SPRankingDownloadPage::getRegionParameterValue() const {
    auto *section = SectionManager::Instance()->currentSection();
    RankingPage::Area area = section->page<PageId::Ranking>()->area();
    assert(area != RankingPage::Area::Friend);

    if (area == RankingPage::Area::Regional) {
        auto *saveManager = System::SaveManager::Instance();
        SP::ClientSettings::RegionLineColor regionLineColorSetting =
                saveManager->getSetting<SP::ClientSettings::Setting::RegionLineColor>();
        if (regionLineColorSetting == SP::ClientSettings::RegionLineColor::Default) {
            return static_cast<s32>(System::SystemManager::Instance()->matchingArea());
        } else {
            return static_cast<s32>(regionLineColorSetting);
        }
    }

    // RankingPage::Area::Worldwide
    return static_cast<s32>(SP::ClientSettings::RegionLineColor::Default);
}

s32 SPRankingDownloadPage::getCourseParameterValue() const {
    auto *section = SectionManager::Instance()->currentSection();
    return static_cast<s32>(section->page<PageId::Ranking>()->course());
}

SPRankingDownloadPage::ResponseStatus SPRankingDownloadPage::responseStatus() const {
    return m_responseStatus;
}

const RankingResponse &SPRankingDownloadPage::rankingResponse() const {
    return m_rankingResponse;
}

SPRankingDownloadPage::State SPRankingDownloadPage::resolve() {
    switch (m_state) {
    case State::Previous:
        break;
    case State::InDevelopment:
        return State::Finished;
    case State::Request:
        return State::Response;
    case State::Response:
        switch (responseStatus()) {
        case ResponseStatus::Ok:
            return State::Finished;
        case ResponseStatus::RequestError:
        case ResponseStatus::ResponseError:
            return State::Previous;
        }
    case State::Finished:
        break;
    }

    return m_state;
}

bool SPRankingDownloadPage::hasRequestTimedOut() const {
    return OSGetTime() >= m_requestTimeout;
}

void SPRankingDownloadPage::requestCallback(NHTTPError error, NHTTPResponseHandle responseHandle) {
    if (error == NHTTP_ERROR_NONE) {
        m_responseStatus = processResponse(responseHandle);
    } else {
        m_responseStatus = ResponseStatus::RequestError;
    }
    NHTTPDestroyResponse(responseHandle);

    auto *section = SectionManager::Instance()->currentSection();
    auto *spinnerAwaitPage = section->page<PageId::SpinnerAwait>();
    assert(section->isPageFocused(spinnerAwaitPage));
    section->page<PageId::SpinnerAwait>()->pop();
}

SPRankingDownloadPage::ResponseStatus SPRankingDownloadPage::processResponse(
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

void *SPRankingDownloadPage::NHTTPAlloc(u32 size, int align) {
    return EGG::TSystem::Instance().eggRootSystem()->alloc(size, align);
}

void SPRankingDownloadPage::NHTTPFree(void *block) {
    EGG::TSystem::Instance().eggRootSystem()->free(block);
}

void SPRankingDownloadPage::RequestCallback(NHTTPError error, NHTTPResponseHandle responseHandle,
        void *arg) {
    auto *spRankingDownloadPage = reinterpret_cast<SPRankingDownloadPage *>(arg);
    spRankingDownloadPage->requestCallback(error, responseHandle);
}

} // namespace UI
