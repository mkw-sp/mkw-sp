#include "RaceManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"

#include <sp/ThumbnailManager.hh>

namespace System {

u8 RaceManager::Player::rank() const {
    return m_rank;
}

u8 RaceManager::Player::maxLap() const {
    return m_maxLap;
}

bool RaceManager::Player::hasFinished() const {
    return m_hasFinished;
}

PadProxy *RaceManager::Player::padProxy() {
    return m_padProxy;
}

RaceManager::Player *RaceManager::player(u32 playerId) {
    return m_players[playerId];
}

void RaceManager::calc() {
    REPLACED(calc)();

    auto *sectionManager = UI::SectionManager::Instance();
    if (sectionManager->currentSection()->id() == UI::SectionId::Thumbnails) {
        if (m_frame == 25) {
            UI::SectionId sectionId;
            if (SP::ThumbnailManager::Continue()) {
                auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
                menuScenario.courseId = SP::ThumbnailManager::CourseId();
                sectionId = UI::SectionId::Thumbnails;
            } else {
                sectionId = UI::SectionId::ServicePack;
            }
            sectionManager->setNextSection(sectionId, UI::Page::Anim::Next);
            sectionManager->startChangeSection(0, 0x000000ff);
        }
    }
}

RaceManager *RaceManager::CreateInstance() {
    s_instance = new RaceManager;
    assert(s_instance);

    s_instance->m_spectatorMode = false;

    return s_instance;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

} // namespace System
