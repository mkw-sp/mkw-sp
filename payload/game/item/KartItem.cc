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

ItemId KartItem::nextItem() {
    switch (m_inventory.getCurrentItem()) {
    case ItemId::TripShrooms:
        return ItemId::Shroom;
        break;
    case ItemId::Shroom:
        return ItemId::Star;
        break;
    case ItemId::Star:
        return ItemId::Golden;
        break;
    case ItemId::Golden:
        return ItemId::Mega;
        break;
    case ItemId::Mega:
        return ItemId::Bill;
        break;
    case ItemId::Bill:
        return ItemId::TC;
        break;
    case ItemId::TC:
        return ItemId::Bomb;
        break;
    case ItemId::Bomb:
        return ItemId::FIB;
        break;
    case ItemId::FIB:
        return ItemId::Nana;
        break;
    case ItemId::Nana:
        return ItemId::Green;
        break;
    case ItemId::Green:
        return ItemId::Shroom;
        break;
    case ItemId::NoItem:
        return ItemId::Shroom;
        break;
    case ItemId::None:
        return ItemId::Shroom;
    default:
        return ItemId::Shroom;
        break;
    }
}

void KartItem::calc() {
    REPLACED(calc)();

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
        } else if (!m_inventory.getPressed()) {
            m_inventory.setItem(nextItem());
            m_inventory.setPressed(true);
        }
    }
}

} // namespace Item
