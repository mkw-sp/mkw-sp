#include "ItemMusicManager.hh"

#include "game/system/SaveManager.hh"

namespace Sound {

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

} // namespace Sound
