#include "KartItem.hh"

#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

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

    if (m_inventory.currentItemID == Item::Golden && m_inventory.framesLeft != 0x1c2) {
        if (m_inventory.framesLeft == 0) {
            m_inventory.currentItemID = Item::NoItem;
        } else {
            m_inventory.framesLeft--;
        }
    }
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ItemWheel>();
    if (setting == SP::ClientSettings::ItemWheel::Enable) {
        auto *playerPadProxy = System::RaceManager::Instance()->player(0)->padProxy();
        auto buttons = playerPadProxy->currentRaceInputState().rawButtons;
        auto controller = playerPadProxy->pad()->getControllerId();
        bool updateItem = false;
        switch (controller) {
        case (Registry::Controller::WiiWheel):
            updateItem = (buttons & PAD_BUTTON_START) == PAD_BUTTON_START;
            break;
        case (Registry::Controller::WiiRemoteAndNunchuck):
            updateItem = (buttons & WPAD_BUTTON_DOWN) == WPAD_BUTTON_DOWN;
            break;
        case (Registry::Controller::Classfic):
            updateItem = (buttons & KPAD_CL_TRIGGER_ZL) == KPAD_CL_TRIGGER_ZL;
            break;
        case (Registry::Controller::GameCube):
            updateItem = (buttons & PAD_BUTTON_Y) == PAD_BUTTON_Y;
            break;
        case (Registry::Controller::None):
            return;
        }
        if (!updateItem) {
            pressedLastFrame = false;
        }

        if (updateItem && !pressedLastFrame) {
            m_inventory.currentItemCount = 100;
            switch (m_inventory.currentItemID) {
            case (Item::TripShrooms):
                m_inventory.currentItemID = Item::Star;
                break;
            case (Item::Star):
                m_inventory.currentItemID = Item::Golden;
                m_inventory.currentItemCount = 1;
                m_inventory.framesLeft = 0x1c2;
                break;
            case (Item::Golden):
                m_inventory.currentItemID = Item::Mega;
                break;
            case (Item::Mega):
                m_inventory.currentItemID = Item::Bill;
                break;
            case (Item::Bill):
                m_inventory.currentItemID = Item::Shroom;
                break;
            case (Item::Shroom):
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

void KartItem::useGolden() {
    REPLACED(useGolden)();
    if (m_inventory.framesLeft == 0x1c2) {
        m_inventory.framesLeft--;
    }
}

} // namespace Item
