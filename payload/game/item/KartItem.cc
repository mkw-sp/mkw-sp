#include "KartItem.hh"

#include "game/system/SaveManager.hh"
#include <game/system/InputManager.hh>
#include <game/system/RaceManager.hh>

#include <game/util/Registry.hh>

#include <sp/cs/RoomClient.hh>

extern "C" {
#include <revolution/kpad.h>
}

static bool pressedLastFrame;

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
        auto *playerPadProxy = System::RaceManager::Instance()->player(0)->padProxy();
        auto buttons = playerPadProxy->currentRaceInputState().rawButtons;
        s32 controller = playerPadProxy->pad()->getControllerId();
        // SP_LOG("buttons: %d    controller: %d", buttons, controller);
        bool updateItem = false;
        switch (controller) {
            // Wanted to use Registry::Controller but getControllerId() returns s32 which i
            // currently don't want to deal with
        case (0): // Wii Wheel
            updateItem = (buttons & PAD_BUTTON_START) == PAD_BUTTON_START;
            break;
        case (1): // Wii Chuck
            updateItem = (buttons & WPAD_BUTTON_DOWN) == WPAD_BUTTON_DOWN;
            break;
        case (2): // Classic
            updateItem = (buttons & KPAD_CL_TRIGGER_ZL) == KPAD_CL_TRIGGER_ZL;
            break;
        case (3): // GCN
            updateItem = (buttons & PAD_BUTTON_Y) == PAD_BUTTON_Y;
            break;
        }
        if (!updateItem) {
            pressedLastFrame = false;
        }

        if (updateItem && !pressedLastFrame) {
            // TODO: Find a better way to keep item after using if at all.
            m_inventory.currentItemCount = 100;
            switch (m_inventory.currentItemID) {
            case (Item::TripShrooms):
                m_inventory.currentItemID = Item::Star;
                break;
            case (Item::Star):
                m_inventory.currentItemID = Item::Golden;
                break;
            case (Item::Golden):
                m_inventory.currentItemID = Item::Mega;
                break;
            case (Item::Mega):
                m_inventory.currentItemID = Item::Bill;
                break;
            case (Item::Bill):
                m_inventory.currentItemID = Item::Green;
                break;
            case (Item::Green):
                m_inventory.currentItemID = Item::FIB;
                break;
            case (Item::FIB):
                m_inventory.currentItemID = Item::Nana;
                break;
            case (Item::Nana):
                m_inventory.currentItemID = Item::TC;
                break;
            case (Item::TC):
                m_inventory.currentItemID = Item::TripShrooms;
                m_inventory.currentItemCount = 3;
                break;
            case (Item::NoItem):
                m_inventory.currentItemID = Item::TripShrooms;
                m_inventory.currentItemCount = 3;
                break;
            }
            pressedLastFrame = true;
        }
    }
}

} // namespace Item
