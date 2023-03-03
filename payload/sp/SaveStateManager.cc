#include "SaveStateManager.hh"

#include "game/kart/KartObjectManager.hh"

extern "C" {
#include "revolution.h"
}
#include <tuple>

namespace SP {

SaveStateManager *SaveStateManager::s_instance = nullptr;

void SaveStateManager::CreateInstance() {
    if (s_instance) {
        return;
    }

    s_instance = new SaveStateManager;
}

void SaveStateManager::DestroyInstance() {
    if (!s_instance) {
        return;
    }

    delete s_instance;
    s_instance = nullptr;
}

auto SaveStateManager::GetKartState() {
    auto kartObjectManager = Kart::KartObjectManager::Instance();
    auto kartObject = kartObjectManager->object(0);
    auto physics = kartObject->getVehiclePhysics();

    return std::make_tuple(kartObject->m_accessor, physics);
}


void SaveStateManager::save() {
    auto [accessor, physics] = GetKartState();
    if (m_kartSaveState.has_value()) {
        (*m_kartSaveState).save(accessor, physics);
    } else {
        m_kartSaveState.emplace(accessor, physics);
    }
}

void SaveStateManager::reload() {
    if (m_kartSaveState.has_value()) {
        auto [accessor, physics] = GetKartState();
        (*m_kartSaveState).reload(accessor, physics);
    } else {
        SP_LOG("SaveStateManager: Reload requested without save!");
    }
}

void SaveStateManager::processInput(bool isPressed) {
    if (!isPressed) {
        if (m_framesHeld == 0) {
            return;
        }

        if (m_framesHeld >= 60) {
            SP_LOG("Reloading!");
            reload();
        } else {
            SP_LOG("Saved!");
            save();
        };

        m_framesHeld = 0;
    } else if (m_framesHeld != 255) {
        m_framesHeld += 1;
    }
}

} // namespace System
