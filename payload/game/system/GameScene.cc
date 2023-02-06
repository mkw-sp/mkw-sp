#include "GameScene.hh"

#include "game/system/SaveManager.hh"

#include <sp/settings/ClientSettings.hh>

extern "C" {
#include <revolution.h>
}

namespace System {

void GameScene::setFramerate(bool is_30) {
    auto setting = SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::FPSMode>();

    switch (setting) {
    case SP::ClientSettings::FPSMode::Vanilla:
        return REPLACED(setFramerate)(is_30);
    case SP::ClientSettings::FPSMode::Force60:
        return REPLACED(setFramerate)(false);
    case SP::ClientSettings::FPSMode::Force30:
        return REPLACED(setFramerate)(true);
    }
}

}
