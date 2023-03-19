#include "OnlineConnectionManagerPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/AwaitPage.hh"
#include "game/ui/GlobalContext.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/OnlineModeSelectPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/OnlineFriendListPage.hh"

#include <protobuf/Matchmaking.pb.h>
#include <sp/cs/RoomClient.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace UI {

constexpr u32 DOLPHIN_DEFAULT_DEVICE_ID = 0x0403AC68;

OnlineConnectionManagerPage::OnlineConnectionManagerPage()
    : m_socket{0x7F000001, 21331, "match   "} {
    m_state = State::Initial;
};

void OnlineConnectionManagerPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
    setAnimSfxIds(0, 0);
}

void OnlineConnectionManagerPage::afterCalc() {
    std::optional<STCMessage> event;
    if (!m_socket.poll()) {
        auto globalContext = SectionManager::Instance()->globalContext();
        globalContext->m_onlineDisconnectInfo.m_category = OnlineErrorCategory::ErrorCode;
        globalContext->m_onlineDisconnectInfo.m_errorCode = 30000;

        changeSection(SectionId::OnlineDisconnected, Anim::None, 0);
    }

    if (!m_socket.ready()) {
        return;
    }

    switch (m_state) {
    case State::Initial:
        return startLogin();
    case State::WaitForLoginChallenge:
        if (!read(event) || !event) {
            return;
        }

        SP_LOG("Got login challenge");
        return respondToChallenge(*event);
    case State::WaitForLoginResponse:
        if (!read(event) || !event) {
            return;
        }

        return handleLoginResponse(*event);
    case State::Idle:
        if (m_searchStarted) {
            SP_LOG("Sending search message");
            sendSearchMessage();
        }

        if (!read(event) || !event) {
            return;
        }

        if (event->which_message == STCMessage_friends_list_tag) {
            SP_LOG("Got friends list");
            return handleFriendsList(event->message.friends_list);
        } else if (event->which_message == STCMessage_found_match_tag) {
            SP_LOG("Got search response");
            return setupMatch(*event);
        } else if (event->which_message == STCMessage_friend_result_tag) {
            SP_LOG("Got friend request response");
            return handleFriendRequestResponse(*event);
        }
    case State::FoundMatch:
        return;
    }
}

std::optional<STCMessage_FoundMatch> OnlineConnectionManagerPage::takeMatchResponse() {
    auto response = m_matchResponse;
    if (response.has_value()) {
        m_state = State::Idle;
        m_searchStarted = false;
    }

    m_matchResponse = std::nullopt;
    return response;
}

void OnlineConnectionManagerPage::startLogin() {
    auto deviceId = GetDeviceId();

    CTSMessage response;
    response.which_message = CTSMessage_login_tag;
    if (deviceId == DOLPHIN_DEFAULT_DEVICE_ID) {
        response.message.login.has_client_id = false;
        m_state = State::WaitForLoginResponse;
    } else {
        response.message.login.has_client_id = true;
        response.message.login.client_id = LoggedInId{
            device : deviceId,
            licence : *System::SaveManager::Instance()->spCurrentLicense(),
        };

        m_state = State::WaitForLoginChallenge;
    }

    write(response);
}

void OnlineConnectionManagerPage::respondToChallenge(const STCMessage & /* event */) {
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
    CTSMessage_LoginChallengeAnswer_challenge_signed_t{size : 0, bytes : {}};

    auto rawMii = globalContext->m_localPlayerMiis.get(0)->id()->getRaw();
    response.message.login_challenge_answer.mii.size = sizeof(System::RawMii);
    memcpy(response.message.login_challenge_answer.mii.bytes, &rawMii, sizeof(System::RawMii));

    m_state = State::WaitForLoginResponse;
    write(response);
}

void OnlineConnectionManagerPage::handleLoginResponse(const STCMessage &event) {
    SP_LOG("Got login response");

    if (event.which_message == STCMessage_response_tag) {
        auto section = SectionManager::Instance()->currentSection();
        auto modeSelectPage = section->page<PageId::OnlineModeSelect>();

        m_vs_rating = event.message.response.vs_rating;
        m_bt_rating = event.message.response.bt_rating;
        modeSelectPage->setRatings(*m_vs_rating, *m_bt_rating);
    }

    m_state = State::Idle;
}

void OnlineConnectionManagerPage::handleFriendsList(const STCMessage_UpdateFriendsList &event) {
    auto section = SectionManager::Instance()->currentSection();
    auto friendListPage = section->page<PageId::OnlineFriendList>();

    m_friends.reset();
    for (u8 i = 0; i < event.friends_count; i += 1) {
        auto &friendInfo = event.friends[i];
        m_friends.push_back(std::move(friendInfo));
    }

    friendListPage->refresh();
}

void OnlineConnectionManagerPage::handleFriendRequestResponse(const STCMessage &event) {
    auto section = SectionManager::Instance()->currentSection();
    auto spinnerPage = section->page<PageId::SPMenuAwait>();

    spinnerPage->setReplacement(PageId::OnlineFriendList);
    spinnerPage->pop();
}

void OnlineConnectionManagerPage::sendSearchMessage() {
    CTSMessage response;
    response.which_message = CTSMessage_start_matchmaking_tag;
    response.message.start_matchmaking.gamemode = m_gamemode;
    response.message.start_matchmaking.trackpack = m_trackpack;

    write(response);
    m_searchStarted = false;
}

void OnlineConnectionManagerPage::setupMatch(const STCMessage &event) {
    m_matchResponse = event.message.found_match;
    m_state = State::FoundMatch;
}

bool OnlineConnectionManagerPage::read(std::optional<STCMessage> &event) {
    u8 buffer[1024];
    STCMessage tmp;

    std::optional<u16> size = m_socket.read(buffer, sizeof(buffer));
    if (!size) {
        return false;
    }

    if (*size == 0) {
        return true;
    }

    pb_istream_t stream = pb_istream_from_buffer(buffer, *size);
    if (!pb_decode(&stream, STCMessage_fields, &tmp)) {
        return false;
    }

    event = tmp;
    return true;
}

void OnlineConnectionManagerPage::write(CTSMessage message) {
    u8 buffer[1024];

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert(pb_encode(&stream, CTSMessage_fields, &message));

    if (!m_socket.write(buffer, stream.bytes_written)) {
        auto globalContext = SectionManager::Instance()->globalContext();
        globalContext->m_onlineDisconnectInfo.m_category = OnlineErrorCategory::ErrorCode;
        globalContext->m_onlineDisconnectInfo.m_errorCode = 30002;

        changeSection(SectionId::OnlineDisconnected, Anim::None, 0);
    }
}

u64 OnlineConnectionManagerPage::getFriendCode(u32 deviceId, u8 licenceId) {
    u64 friendCode = 0;
    friendCode |= (u64)deviceId << 0;
    friendCode |= (u64)licenceId << 32;
    return friendCode;
}

u32 OnlineConnectionManagerPage::GetDeviceId() {
    u32 deviceId;
    s32 deviceIdRes = ESP_GetDeviceId(&deviceId);
    if (deviceIdRes != 0) {
        panic("Failed to get device id: %d", deviceIdRes);
    };

    return deviceId;
}

void OnlineConnectionManagerPage::requestRegisterFriend(u32 deviceId, u8 licenceId, bool isDelete) {
    CTSMessage message;
    message.which_message = CTSMessage_friend_request_tag;
    message.message.friend_request.is_delete = isDelete;
    message.message.friend_request.licenceId = licenceId;
    message.message.friend_request.deviceId = deviceId;

    write(message);
}

} // namespace UI
