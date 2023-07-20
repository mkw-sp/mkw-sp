#include "OnlineConnectionManagerPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/OnlineModeSelectPage.hh"
#include "game/ui/SectionManager.hh"

#include <protobuf/Matchmaking.pb.h>

namespace UI {

constexpr u32 DOLPHIN_DEFAULT_DEVICE_ID = 0x0403AC68;

OnlineConnectionManagerPage::OnlineConnectionManagerPage()
    : m_innerSocket{0x7F000001, 21331, "match   "}, m_socket{&m_innerSocket, STCMessage_fields,
                                                            CTSMessage_fields} {
    m_state = State::Initial;
};

void OnlineConnectionManagerPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
    setAnimSfxIds(Sound::SoundId::SE_DUMMY, Sound::SoundId::SE_DUMMY);
}

void OnlineConnectionManagerPage::afterCalc() {
    auto *sectionManager = SectionManager::Instance();
    if (!m_socket.inner().poll()) {
        sectionManager->transitionToError(30000);
    }

    if (!m_socket.inner().ready()) {
        return;
    }

    auto res = transition();
    if (!res.has_value()) {
        MessageInfo info;
        info.strings[0] = res.error();
        sectionManager->transitionToError(30002, info);
    }
}

std::expected<void, const wchar_t *> OnlineConnectionManagerPage::transition() {
    std::expected<std::optional<STCMessage>, const wchar_t *> event;

    switch (m_state) {
    case State::Initial:
        return startLogin();
    case State::WaitForLoginChallenge:;
        if ((event = TRY(m_socket.readProto()))) {
            SP_LOG("Got login challenge");
            return respondToChallenge(**event);
        } else {
            return {};
        }
    case State::WaitForLoginResponse:
        if ((event = TRY(m_socket.readProto()))) {
            SP_LOG("Got login response");
            setupRatings(**event);
        }

        return {};
    case State::WaitForSearchStart:
        if (m_searchStarted) {
            SP_LOG("Sending search message");
            return sendSearchMessage();
        } else {
            return {};
        }
    case State::WaitForSearchResponse:
        if ((event = TRY(m_socket.readProto()))) {
            SP_LOG("Got search response");
            setupMatch(**event);
        }

        return {};
    case State::FoundMatch:
        return {};
    };

    panic("unreachable");
}

std::optional<STCMessage_FoundMatch> OnlineConnectionManagerPage::takeMatchResponse() {
    auto response = m_matchResponse;
    if (response.has_value()) {
        m_state = State::WaitForSearchStart;
        m_searchStarted = false;
    }

    m_matchResponse = std::nullopt;
    return response;
}

std::expected<void, const wchar_t *> OnlineConnectionManagerPage::startLogin() {
    CTSMessage response;

    u32 deviceId;
    s32 deviceIdRes = ESP_GetDeviceId(&deviceId);
    if (deviceIdRes != 0) {
        panic("Failed to get device id: %d", deviceIdRes);
    }

    response.which_message = CTSMessage_login_tag;
    if (deviceId == DOLPHIN_DEFAULT_DEVICE_ID) {
        response.message.login.has_client_id = false;
        m_state = State::WaitForLoginResponse;
    } else {
        response.message.login.has_client_id = true;
        response.message.login.client_id = LoggedInId{
                .device = deviceId,
                .licence = *System::SaveManager::Instance()->spCurrentLicense(),
        };

        m_state = State::WaitForLoginChallenge;
    }

    return m_socket.writeProto(response);
}

std::expected<void, const wchar_t *> OnlineConnectionManagerPage::respondToChallenge(
        const STCMessage & /* event */) {
    CTSMessage response;
    u16 longitude;
    u16 latitude;
    u32 location;

    auto *saveManager = System::SaveManager::Instance();
    auto globalContext = UI::SectionManager::Instance()->globalContext();

    saveManager->getLongitude(&longitude);
    saveManager->getLocation(&location);
    saveManager->getLatitude(&latitude);

    response.which_message = CTSMessage_login_challenge_answer_tag;

    response.message.login_challenge_answer.location = location;
    response.message.login_challenge_answer.latitude = latitude;
    response.message.login_challenge_answer.longitude = longitude;
    response.message.login_challenge_answer.challenge_signed =
            CTSMessage_LoginChallengeAnswer_challenge_signed_t{.size = 0, .bytes = {}};

    auto rawMii = globalContext->m_localPlayerMiis.get(0)->id()->getRaw();
    response.message.login_challenge_answer.mii.size = sizeof(System::RawMii);
    memcpy(response.message.login_challenge_answer.mii.bytes, &rawMii, sizeof(System::RawMii));

    m_state = State::WaitForLoginResponse;
    return m_socket.writeProto(response);
}

void OnlineConnectionManagerPage::setupRatings(const STCMessage &event) {
    if (event.which_message == STCMessage_response_tag) {
        auto section = SectionManager::Instance()->currentSection();
        auto modeSelectPage = section->page<PageId::OnlineModeSelect>();

        m_vs_rating = event.message.response.vs_rating;
        m_bt_rating = event.message.response.bt_rating;
        modeSelectPage->setRatings(*m_vs_rating, *m_bt_rating);
    }

    m_state = State::WaitForSearchStart;
}

std::expected<void, const wchar_t *> OnlineConnectionManagerPage::sendSearchMessage() {
    CTSMessage response;
    response.which_message = CTSMessage_start_matchmaking_tag;
    response.message.start_matchmaking.gamemode = m_gamemode;
    response.message.start_matchmaking.trackpack = m_trackpack;

    m_searchStarted = false;
    m_state = State::WaitForSearchResponse;
    return m_socket.writeProto(response);
}

void OnlineConnectionManagerPage::setupMatch(const STCMessage &event) {
    if (event.which_message != STCMessage_found_match_tag) {
        panic("unexpected event");
    }

    m_matchResponse = event.message.found_match;
    m_state = State::FoundMatch;
}

} // namespace UI
