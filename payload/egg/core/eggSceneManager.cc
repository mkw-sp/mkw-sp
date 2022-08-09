#include "eggSceneManager.hh"

extern "C" {
#include <game/system/ResourceManager.h>
#include <revolution.h>
}
#include <sp/IOSDolphin.hh>

namespace EGG {

bool SceneManager::s_dolphinIsUnavailable = false;
u32 SceneManager::s_dolphinSpeedStack[8];
s32 SceneManager::s_dolphinSpeedStackSize;

bool SceneManager::InitDolphinSpeed() {
    if (s_dolphinIsUnavailable) {
        return false;
    }

    if (SP::IOSDolphin::IsOpen()) {
        return true;
    }

    s_dolphinIsUnavailable = !SP::IOSDolphin::Open();
    return !s_dolphinIsUnavailable;
}

bool SceneManager::SetDolphinSpeed(u32 percent) {
    if (SP::IOSDolphin::Open()) {
        SP_LOG("Set Dolphin speed to %u", percent);
        return SP::IOSDolphin::SetSpeedLimit(percent) == IPC_OK;
    }
    return false;
}

u32 SceneManager::GetDolphinSpeedLimit() {
    if (!SP::IOSDolphin::Open())
        return 0;

    auto q = SP::IOSDolphin::GetSpeedLimit();
    if (!q.has_value())
        return 0;

    return *q;
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
    ResourceManager_OnCreateScene(sceneId);
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
