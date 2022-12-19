#include "RaceManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"

#include <sp/ThumbnailManager.hh>
#include <sp/cs/RaceManager.hh>

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

void RaceManager::Player::setExtraGhostPadProxy() {
    m_padProxy = InputManager::Instance()->extraGhostProxy(m_playerId);
}

RaceManager::Player *RaceManager::player(u32 playerId) {
    return m_players[playerId];
}

u32 RaceManager::frameId() const {
    return m_frameId;
}

void RaceManager::calc() {
    REPLACED(calc)();

    auto *sectionManager = UI::SectionManager::Instance();
    if (sectionManager->currentSection()->id() == UI::SectionId::Thumbnails) {
        if (m_frameId == 25) {
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

void RaceManager::endPlayerRace(u32 playerId) {
    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    if (raceScenario.players[playerId].type == RaceConfig::Player::Type::Ghost) {
        InputManager::Instance()->endExtraGhostProxy(playerId);
    }

    REPLACED(endPlayerRace)(playerId);
}

RaceManager *RaceManager::CreateInstance() {
    s_instance = new RaceManager;
    assert(s_instance);

    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    for (u32 i = 0; i < raceScenario.playerCount; i++) {
        if (raceScenario.players[i].type == RaceConfig::Player::Type::Ghost) {
            s_instance->m_players[i]->setExtraGhostPadProxy();
        }

        if (auto *roomManager = SP::RoomManager::Instance(); roomManager &&
                !roomManager->isPlayerLocal(i)) {
            InputManager::Instance()->resetExtraGhostProxy(i);
            s_instance->m_players[i]->setExtraGhostPadProxy();
        }
    }

    s_instance->m_spectatorMode = false;
    if (SP::RaceManager::Instance()) {
        s_instance->m_canStartCountdown = false;
    }

    return s_instance;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

} // namespace System
