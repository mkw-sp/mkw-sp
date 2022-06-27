#include "eggSceneManager.hh"

extern "C" {
#include <game/system/ResourceManager.h>
#include <revolution.h>
}

namespace EGG {

bool SceneManager::s_dolphinIsUnavailable = false;
IOSDolphin SceneManager::s_dolphin = -1;
u32 SceneManager::s_dolphinSpeedStack[8];
s32 SceneManager::s_dolphinSpeedStackSize;

bool SceneManager::InitDolphinSpeed() {
    if (s_dolphinIsUnavailable) {
        return false;
    }

    if (s_dolphin >= 0) {
        return true;
    }

    s_dolphin = IOSDolphin_Open();
    s_dolphinIsUnavailable = s_dolphin < 0;
    return !s_dolphinIsUnavailable;
}

bool SceneManager::SetDolphinSpeed(u32 percent) {
    if (s_dolphin >= 0) {
        SP_LOG("Set Dolphin speed to %u", percent);
        return IOSDolphin_SetSpeedLimit(s_dolphin, percent);
    }
    return false;
}

u32 SceneManager::GetDolphinSpeedLimit() {
    if (s_dolphin < 0)
        return 0;

    IOSDolphin_SpeedLimitQuery q = IOSDolphin_GetSpeedLimit(s_dolphin);
    if (!q.hasValue)
        return 0;

    return q.emulationSpeedPercent;
}

void SceneManager::PushDolphinSpeed(u32 percent) {
    if (s_dolphinSpeedStackSize == 8) {
        SP_LOG("Max Dolphin speed stack depth reached");
        return;
    }

    const u32 oldLimit = GetDolphinSpeedLimit();
    if (oldLimit == 0) {
        SP_LOG("Failed to acquire current Dolphin speed");
        return;
    }

    if (SetDolphinSpeed(percent)) {
        s_dolphinSpeedStack[s_dolphinSpeedStackSize++] = oldLimit;
    } else {
        SP_LOG("Failed to set Dolphin speed");
    }
}

void SceneManager::PopDolphinSpeed() {
    if (s_dolphinSpeedStackSize > 0) {
        SetDolphinSpeed(s_dolphinSpeedStack[--s_dolphinSpeedStackSize]);
    }
}

void SceneManager::reinitCurrentScene() {
    if (InitDolphinSpeed()) {
        PushDolphinSpeed(800);
    }
    REPLACED(reinitCurrentScene)();
    if (InitDolphinSpeed()) {
        PopDolphinSpeed();
    }
}

void SceneManager::createScene(s32 sceneId, Scene *parent) {
    if (InitDolphinSpeed()) {
        PushDolphinSpeed(800);
    }
    ResourceManager_onCreateScene(sceneId);
    REPLACED(createScene)(sceneId, parent);
    if (InitDolphinSpeed()) {
        PopDolphinSpeed();
    }
}

void SceneManager::destroyScene(Scene *scene) {
    if (InitDolphinSpeed()) {
        PushDolphinSpeed(800);
    }
    REPLACED(destroyScene)(scene);
    if (InitDolphinSpeed()) {
        PopDolphinSpeed();
    }
}

} // namespace EGG
