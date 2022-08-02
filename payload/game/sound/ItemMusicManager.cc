#include "ItemMusicManager.hh"

#include "game/sound/RaceSoundManager.hh"
#include "game/system/SaveManager.hh"

namespace Sound {

ItemMusicManager *ItemMusicManager::CreateInstance() {
    auto *instance = REPLACED(CreateInstance)();
    instance->m_speedup = false;
    return instance;
}

ItemMusicManager *ItemMusicManager::Instance() {
    return s_instance;
}

void ItemMusicManager::resolve() {
    REPLACED(resolve)();

    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ItemMusic>();

    switch (m_state) {
    case State::KinokoBig:
    case State::Star:
        if (setting != SP::ClientSettings::ItemMusic::All) {
            m_state = State::Idle;
        }
        break;
    case State::Damage:
        if (setting == SP::ClientSettings::ItemMusic::None) {
            m_state = State::Idle;
        }
        break;
    default:
        break;
    }
}

f32 ItemMusicManager::pitch() const {
    u32 state = RaceSoundManager::Instance()->state();
    if (state < 4 || state > 6) {
        return m_pitch;
    }

    if (!m_speedup) {
        return m_pitch;
    }

    return m_pitch * (9.0f / 8.0f); // One major tone
}

} // namespace Sound
