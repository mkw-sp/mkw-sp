#include "RandomMatchingPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <protobuf/Matchmaking.pb.h>
#include <sp/cs/RoomClient.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace UI {

constexpr u32 DOLPHIN_DEFAULT_DEVICE_ID = 0x0403AC68;

RandomMatchingPage::RandomMatchingPage() : m_socket{0x7F000001, 21331, "match   "} {
    m_state = State::Login;
};

PageId RandomMatchingPage::getReplacement() {
    return PageId::FriendMatching;
}

void RandomMatchingPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
    setAnimSfxIds(0, 0);
}

void RandomMatchingPage::afterCalc() {
    std::optional<STCMessage> event;
    assert(m_socket.poll());
    if (!m_socket.ready()) {
        return;
    }

    switch (m_state) {
    case State::Login:
        return respondToLogin();
    case State::WaitForChallenge:
        if (!read(event) || !event) {
            return;
        }

        return respondToChallenge(*event);
    case State::WaitForResponse:
        if (!read(event) || !event) {
            return;
        }

        return respondToResponse(*event);
    case State::WaitForMatch:
        if (!read(event) || !event) {
            return;
        }

        return transitionToRoom(*event);
    case State::Transitioning:
        return;
    default:
        panic("unimplemented");
    }
}

void RandomMatchingPage::respondToLogin() {
    CTSMessage response;

    u32 deviceId;
    s32 deviceIdRes = ESP_GetDeviceId(&deviceId);
    if (deviceIdRes != 0) {
        panic("Failed to get device id: %d", deviceIdRes);
    }

    response.which_message = CTSMessage_login_tag;
    if (deviceId == DOLPHIN_DEFAULT_DEVICE_ID) {
        response.message.login.has_client_id = false;
        m_state = State::WaitForResponse;
    } else {
        response.message.login.has_client_id = true;
        response.message.login.client_id = LoggedInId{
            device : deviceId,
            licence : *System::SaveManager::Instance()->spCurrentLicense(),
        };

        m_state = State::WaitForChallenge;
    }

    assert(write(response));
}

void RandomMatchingPage::respondToChallenge(const STCMessage &event) {
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

    m_state = State::WaitForResponse;
    assert(write(response));
}

void RandomMatchingPage::respondToResponse(const STCMessage &event) {
    CTSMessage response;

    switch (event.which_message) {
    case STCMessage_response_tag:
    case STCMessage_guest_response_tag:
        response.which_message = CTSMessage_start_matchmaking_tag;
        response.message.start_matchmaking.gamemode = 0;

        m_state = State::WaitForMatch;
        break;
    default:
        panic("unexpected event");
    }

    assert(write(response));
}

void RandomMatchingPage::transitionToRoom(const STCMessage &event) {
    if (event.which_message != STCMessage_found_match_tag) {
        panic("unexpected event");
    }

    auto port = 21330;
    auto ip = event.message.found_match.room_ip;
    auto loginInfo = event.message.found_match.login_info;

    SP::RoomClient::CreateInstance(1, ip, port, 0000, loginInfo);
    startReplace(Anim::Next, 0);

    m_state = State::Transitioning;
}

bool RandomMatchingPage::read(std::optional<STCMessage> &event) {
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

bool RandomMatchingPage::write(CTSMessage message) {
    u8 buffer[1024];

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert(pb_encode(&stream, CTSMessage_fields, &message));
    return m_socket.write(buffer, stream.bytes_written);
}

} // namespace UI
