#include "GameScene.hh"

#include "game/system/SaveManager.hh"

#include <egg/core/eggXfbManager.hh>
#include <sp/ScreenshotManager.hh>
#include <sp/settings/ClientSettings.hh>

namespace System {

void GameScene::calc() {
    REPLACED(calc)();

    SP::ScreenshotManager::Instance()->calc();
}

void GameScene::draw() {
    REPLACED(draw)();

    SP::ScreenshotManager::Instance()->draw();
}

void GameScene::setFramerate(bool is30FPS) {
    auto setting = SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::FPSMode>();

    switch (setting) {
    case SP::ClientSettings::FPSMode::Vanilla:
        return REPLACED(setFramerate)(is30FPS);
    case SP::ClientSettings::FPSMode::Force60:
        return REPLACED(setFramerate)(false);
    case SP::ClientSettings::FPSMode::Force30:
        return REPLACED(setFramerate)(true);
    }
}

} // namespace System
