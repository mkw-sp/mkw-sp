#include "ItemInventory.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"

extern "C" {
#include <revolution.h>
}

namespace Item {

void ItemInventory::resetItem() {
    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::YButton>();
    if (setting == SP::ClientSettings::YButton::ItemWheel &&
            gameMode == System::RaceConfig::GameMode::TimeAttack) {
        return;
    }
    REPLACED(resetItem)();
}

void ItemInventory::resetHeldItem() {
    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::YButton>();
    if (setting == SP::ClientSettings::YButton::ItemWheel &&
            gameMode == System::RaceConfig::GameMode::TimeAttack) {
        return;
    }
    REPLACED(resetHeldItem)();
}

ItemId ItemInventory::getCurrentItem() const {
    return m_currentItemID;
}

void ItemInventory::setItemWheelPressed(bool pressed) {
    m_itemWheelPressed = pressed;
}

bool ItemInventory::getItemWheelPressed() const {
    return m_itemWheelPressed;
}

} // namespace Item
