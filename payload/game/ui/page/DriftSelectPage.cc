#include "DriftSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include <sp/cs/RoomClient.hh>

namespace UI {

void DriftSelectPage::onButtonFront(PushButton *button) {
    auto *client = SP::RoomClient::Instance();
    if (client) {
        auto player = System::RaceConfig::Instance()->menuScenario().players[0];
        SP_LOG("Client: Properties sent! %d, %d, %d", player.characterId, player.vehicleId, button->m_index == 1);
        client->selectProperties(player.characterId, player.vehicleId, button->m_index == 1);
    }
    REPLACED(onButtonFront)(button);
}

} // namespace UI
