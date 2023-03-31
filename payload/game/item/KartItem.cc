#include "KartItem.hh"

#include <game/system/InputManager.hh>
#include "game/system/SaveManager.hh"

#include <sp/cs/RoomClient.hh>

extern "C" {
    #include <revolution/kpad.h>
}

namespace Item {

void KartItem::setItem(u32 r4, u32 r5, u32 r6) {
    if (auto *roomClient = SP::RoomClient::Instance();
            roomClient && roomClient->isPlayerRemote(m_playerId)) {
        return;
    }

    REPLACED(setItem)(r4, r5, r6);
}

void KartItem::update() {
    REPLACED(update)();
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ItemWheel>();
    if (setting == SP::ClientSettings::ItemWheel::Enable) {
        auto inputManager = System::InputManager::Instance();
        bool yPressed = inputManager->getYPressed();
        bool yLock = inputManager->getYLock();
        if (yPressed && !yLock) {
            SP_LOG("Y button pressed and lock is false");
            //inventory.currentItemCount = 7; TODO: Find a better way to keep item after using.
            switch (inventory.currentItemID) {
            case (Item::TripShrooms):
                inventory.currentItemID = Item::Star;
                break;
            case (Item::Star):
                inventory.currentItemID = Item::Golden;
                break;
            case (Item::Golden):
                inventory.currentItemID = Item::Mega;
                break;
            case (Item::Mega):
                inventory.currentItemID = Item::Bill;
                break;
            case (Item::Bill):
                inventory.currentItemID = Item::TripShrooms;
                inventory.currentItemCount = 3;
                break;
            case (Item::NoItem):
                inventory.currentItemID = Item::TripShrooms;
                inventory.currentItemCount = 3;
                break;
            }
        }
    }
}

} // namespace Item
