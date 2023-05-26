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
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ItemWheel>();
    if (setting == SP::ClientSettings::ItemWheel::Enable &&
            gameMode == System::RaceConfig::GameMode::TimeAttack) {
        return;
    }
    REPLACED(resetItem)();
}

void ItemInventory::resetHeldItem() {
    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ItemWheel>();
    if (setting == SP::ClientSettings::ItemWheel::Enable &&
            gameMode == System::RaceConfig::GameMode::TimeAttack) {
        return;
    }
    REPLACED(resetHeldItem)();
}

Items ItemInventory::getItem() {
    return m_currentItemID;
}

void ItemInventory::setPressed(bool pressed) {
    m_pressedLastFrame = pressed;
}

bool ItemInventory::getPressed() {
    return m_pressedLastFrame;
}

} // namespace Item
