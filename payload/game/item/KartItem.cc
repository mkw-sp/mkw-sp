#include "KartItem.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"
#include "game/util/Registry.hh"

extern "C" {
#include <revolution/kpad.h>
}
#include <sp/cs/RoomClient.hh>

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

    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ItemWheel>();

    if (gameMode == System::RaceConfig::GameMode::TimeAttack &&
            setting == SP::ClientSettings::ItemWheel::Enable) {
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
        case (Registry::Controller::Classic):
            updateItem = (buttons & KPAD_CL_TRIGGER_ZL) == KPAD_CL_TRIGGER_ZL;
            break;
        case (Registry::Controller::GameCube):
            updateItem = (buttons & PAD_BUTTON_Y) == PAD_BUTTON_Y;
            break;
        case (Registry::Controller::None):
            return;
        }
        if (!updateItem) {
            m_inventory.setPressed(false);
        }

        if (updateItem && !m_inventory.getPressed()) {
            Item::Items nextItem = Item::NoItem;
            switch (m_inventory.getItem()) {
            case (Item::TripShrooms):
                nextItem = Item::Shroom;
                break;
            case (Item::Shroom):
                nextItem = Item::Star;
                break;
            case (Item::Star):
                nextItem = Item::Golden;
                break;
            case (Item::Golden):
                nextItem = Item::Mega;
                break;
            case (Item::Mega):
                nextItem = Item::Bill;
                break;
            case (Item::Bill):
                nextItem = Item::TC;
                break;
            case (Item::TC):
                nextItem = Item::Bomb;
                break;
            case (Item::Bomb):
                nextItem = Item::FIB;
                break;
            case (Item::FIB):
                nextItem = Item::Nana;
                break;
            case (Item::Nana):
                nextItem = Item::Green;
                break;
            case (Item::Green):
                nextItem = Item::Shroom;
                break;
            case (Item::NoItem):
                nextItem = Item::Shroom;
                break;
            default:
                nextItem = Item::Shroom;
                break;
            case (Item::None):
                return;
            }
            m_inventory.setItem(nextItem);
            m_inventory.setPressed(true);
        }
    }
}

} // namespace Item
