#include "Page.hh"
#include "sp/net/AsyncSocket.hh"
#include <protobuf/Matchmaking.pb.h>

namespace UI {

class MatchmakingPage : public Page {
public:
    MatchmakingPage();
    void onInit() override;
    void afterCalc() override;
    PageId getReplacement() override;

private:
    enum class State {
        Login,
        WaitForChallenge,
        WaitForResponse,
        WaitForMatch,
        Transitioning,
    };

    bool read(std::optional<STCMessage> &event);
    bool write(CTSMessage message);

    void respondToLogin();
    void respondToChallenge(const STCMessage &event);
    void transitionToRoom(const STCMessage &event);

    PageInputManager m_inputManager;
    SP::Net::AsyncSocket m_socket;
    State m_state;
};

}
