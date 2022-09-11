#include "DriftSelectPage.hh"

#include "game/ui/SectionManager.hh"
#include <sp/cs/RoomClient.hh>

namespace UI {

void DriftSelectPage::onButtonFront(PushButton *button) {
    auto *client = SP::RoomClient::Instance();
    if (client) {
        auto player = SectionManager::Instance()->globalContext()->m_selectPlayer[0];
        SP_LOG("Client: Properties sent! %d, %d, %d", player.m_characterId, player.m_vehicleId, button->m_index == 1);
        client->selectProperties(player.m_characterId, player.m_vehicleId, button->m_index == 1);
    }
    REPLACED(onButtonFront)(button);
}

} // namespace UI
