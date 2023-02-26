#include "SaveStateManager.hh"

#include "game/kart/KartObjectManager.hh"

extern "C" {
#include "revolution.h"
}
#include <tuple>

namespace SP {

SaveStateManager *SaveStateManager::s_instance = nullptr;

SaveStateManager::SaveStateManager() {
    m_framesHeld = 0;
    m_hasSaved = false;
    m_kartSaveState = new Kart::KartSaveState;
}

void SaveStateManager::CreateInstance() {
    if (s_instance) {
        return;
    }

    s_instance = new SaveStateManager();
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
    m_kartSaveState->save(accessor, physics);
    m_hasSaved = true;
}

void SaveStateManager::reload() {
    if (!m_hasSaved) {
        SP_LOG("SaveStateManager: Reload requested without save!");
        return;
    }

    auto [accessor, physics] = GetKartState();
    m_kartSaveState->reload(accessor, physics);
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
