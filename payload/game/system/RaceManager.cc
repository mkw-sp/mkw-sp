#include "RaceManager.hh"

#include "game/gfx/CameraManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"

#include <sp/ThumbnailManager.hh>
#include <sp/cs/RaceManager.hh>

namespace System {

u8 RaceManager::Player::rank() const {
    return m_rank;
}

u16 RaceManager::Player::battleScore() const {
    return m_battleScore;
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

Util::Random *RaceManager::dynamicRandom() {
    return m_dynamicRandom;
}

RaceManager::Player *RaceManager::player(u32 playerId) {
    return m_players[playerId];
}

u32 RaceManager::time() const {
    return m_time;
}

MapdataKartPoint *RaceManager::getKartPoint(u32 playerId) {
    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    if (raceScenario.gameMode != RaceConfig::GameMode::OfflineBT) {
        return REPLACED(getKartPoint)(playerId);
    }

    s16 id = playerId;
    if (raceScenario.spMaxTeamSize >= 2) {
        id = 0;
        for (u32 i = 0; i < playerId; i++) {
            if (raceScenario.players[i].spTeam == raceScenario.players[playerId].spTeam) {
                id++;
            }
        }
        id = (id + m_battleKartPointStart) % raceScenario.spMaxTeamSize;
        id += raceScenario.players[playerId].spTeam * raceScenario.spMaxTeamSize;
    }

    auto *courseMap = CourseMap::Instance();
    u32 index;
    for (index = 0; index < courseMap->kartPoint()->m_numEntries; index++) {
        if (courseMap->kartPoint()->m_entryAccessors[index]->m_data->id == id) {
            break;
        }
    }

    if (index == courseMap->kartPoint()->m_numEntries) {
        return nullptr;
    } else {
        return courseMap->kartPoint()->m_entryAccessors[index];
    }
}

void RaceManager::getStartTransform(Vec3 *pos, Vec3 *rot, u32 playerId) {
    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    if (raceScenario.gameMode != RaceConfig::GameMode::OfflineBT) {
        REPLACED(getStartTransform)(pos, rot, playerId);
        return;
    }

    if (auto *kartPoint = getKartPoint(playerId)) {
        kartPoint->getTransform(pos, rot, 1, 1);
    } else {
        *pos = {0.0f, 0.0f, 0.0f};
        *rot = {1.0f, 0.0f, 0.0f}; // Sneaky 1 for some reason
    }
}

void RaceManager::calc() {
    REPLACED(calc)();

    auto *sectionManager = UI::SectionManager::Instance();
    if (sectionManager->currentSection()->id() == UI::SectionId::Thumbnails) {
        if (m_time == 25) {
            UI::SectionId sectionId;
            if (SP::ThumbnailManager::Continue()) {
                auto *raceConfig = System::RaceConfig::Instance();
                raceConfig->m_menuScenario.courseId = SP::ThumbnailManager::CourseId();
                raceConfig->m_spMenu.pathReplacement = SP::ThumbnailManager::Path();
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
