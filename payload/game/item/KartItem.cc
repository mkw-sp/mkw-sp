#include "KartItem.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include "game/util/Registry.hh"

#include <sp/cs/RoomClient.hh>

#include <algorithm>

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
    const std::array<ItemId, 10> itemCycle{ItemId::Shroom, ItemId::Golden, ItemId::Star,
            ItemId::Mega, ItemId::Bill, ItemId::Nana, ItemId::FIB, ItemId::Green, ItemId::Bomb,
            ItemId::TC};

    auto it = std::find(std::begin(itemCycle), std::end(itemCycle), m_inventory.getCurrentItem());
    if (it != itemCycle.end() && it != itemCycle.begin() + (itemCycle.size() - 1)) {
        return *(it + 1);
    }
    return itemCycle.front();
}

void KartItem::calc() {
    REPLACED(calc)();

    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::YButton>();

    if (gameMode == System::RaceConfig::GameMode::TimeAttack &&
            setting == SP::ClientSettings::YButton::ItemWheel) {
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
            m_inventory.setItemWheelPressed(false);
        } else if (!m_inventory.getItemWheelPressed()) {
            m_inventory.setItem(nextItem());
            m_inventory.setItemWheelPressed(true);
        }
    }
}

} // namespace Item
