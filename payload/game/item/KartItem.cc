#include "KartItem.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include "game/util/Registry.hh"

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
        case Registry::Controller::WiiWheel:
            updateItem = (buttons & PAD_BUTTON_START) == PAD_BUTTON_START;
            break;
        case Registry::Controller::WiiRemoteAndNunchuck:
            updateItem = (buttons & WPAD_BUTTON_DOWN) == WPAD_BUTTON_DOWN;
            break;
        case Registry::Controller::Classic:
            updateItem = (buttons & KPAD_CL_TRIGGER_ZL) == KPAD_CL_TRIGGER_ZL;
            break;
        case Registry::Controller::GameCube:
            updateItem = (buttons & PAD_BUTTON_Y) == PAD_BUTTON_Y;
            break;
        case Registry::Controller::None:
            return;
        }
        if (!updateItem) {
            m_inventory.setPressed(false);
        }

        if (updateItem && !m_inventory.getPressed()) {
            Item::Items nextItem = Items::NoItem;
            switch (m_inventory.getItem()) {
            case Items::TripShrooms:
                nextItem = Items::Shroom;
                break;
            case Items::Shroom:
                nextItem = Items::Star;
                break;
            case Items::Star:
                nextItem = Items::Golden;
                break;
            case Items::Golden:
                nextItem = Items::Mega;
                break;
            case Items::Mega:
                nextItem = Items::Bill;
                break;
            case Items::Bill:
                nextItem = Items::TC;
                break;
            case Items::TC:
                nextItem = Items::Bomb;
                break;
            case Items::Bomb:
                nextItem = Items::FIB;
                break;
            case Items::FIB:
                nextItem = Items::Nana;
                break;
            case Items::Nana:
                nextItem = Items::Green;
                break;
            case Items::Green:
                nextItem = Items::Shroom;
                break;
            case Items::NoItem:
                nextItem = Items::Shroom;
                break;
            case Items::None:
                return;
            default:
                nextItem = Items::Shroom;
                break;
            }
            m_inventory.setItem(nextItem);
            m_inventory.setPressed(true);
        }
    }
}

} // namespace Item
