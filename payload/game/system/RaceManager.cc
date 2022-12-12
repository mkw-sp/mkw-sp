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

void RaceManager::setSpectatorMode(bool toggle) {
    m_spectatorMode = toggle;
}

bool RaceManager::spectatorMode() const {
    return m_spectatorMode;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

RaceManager::Mode *RaceManager::initMode(RaceConfig::GameMode mode) {
    switch (mode) {
    case RaceConfig::GameMode::OnlineClient:
        return new OnlineClient(this);
    case RaceConfig::GameMode::OnlineServer:
        return new OnlineServer(this);
    default:
        return REPLACED(initMode)(mode);
    }
}

RaceManager::Mode::Mode(RaceManager *manager) : m_manager(manager) {}

// HACK: we need the virtual function offsets to match
void RaceManager::Mode::vf_00() {}

RaceManager::OnlineClient::OnlineClient(RaceManager *manager) : Mode(manager) {}

void RaceManager::OnlineClient::calc() {}

RaceManager::OnlineServer::OnlineServer(RaceManager *manager) : Mode(manager) {}

void RaceManager::OnlineServer::calc() {}

} // namespace System
