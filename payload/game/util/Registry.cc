#include "Registry.hh"

extern "C" {
#include "Registry.h"
}

#include "game/system/SaveManager.hh"

namespace Registry {

static bool IsCharacterValid(Character character) {
    switch (character) {
    case Character::Mario... Character::SmallMiiOutfitBFemale:
    case Character::MediumMiiOutfitAMale... Character::MediumMiiOutfitBFemale:
    case Character::LargeMiiOutfitAMale... Character::LargeMiiOutfitBFemale:
    case Character::MediumMii... Character::LargeMii:
        return true;
    default:
        return false;
    }
}

static bool IsVehicleValid(Vehicle vehicle) {
    switch (vehicle) {
    case Vehicle::StandardKartSmall... Vehicle::Phantom:
        return true;
    default:
        return false;
    }
}

bool IsCombinationValid(Character character, Vehicle vehicle) {
    if (!IsCharacterValid(character)) {
        return false;
    }
    if (!IsVehicleValid(vehicle)) {
        return false;
    }

    return getCharacterWeightClass(static_cast<u32>(character)) ==
            getVehicleWeightClass(static_cast<u32>(vehicle));
}

// This disables trying to load red/blue thumbnails and models
// for all the vehicles in the vehicle select screen
bool UseBattleRenders() {
    return false;
}

const char *GetItemPane(u32 itemId, u32 count) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();

    // Return mega TC pane
    if (itemId == 0x0E && setting == SP::ClientSettings::VSMegaClouds::Enable) {
        return "mega_c";
    }

    return REPLACED(GetItemPane)(itemId, count);
}

} // namespace Registry
