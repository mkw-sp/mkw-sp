#include "CtrlRaceNameBalloon.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/TeamColors.hh"
#include "game/ui/page/RacePage.hh"
#include "game/util/Registry.hh"

#include <sp/cs/RoomManager.hh>
#include <sp/settings/RegionLineColor.hh>

extern float playerTagRenderDistance;

namespace UI {

void CtrlRaceNameBalloon::calcVisibility() {
    // An artificial 1 frame delay is added to match the camera.
    const auto *racePage = getPage()->downcast<RacePage>();
    assert(racePage);
    u8 lastWatchedPlayerId = racePage->lastWatchedPlayerId();

    if (!getVisible()) {
        return;
    }

    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.gameMode != System::RaceConfig::GameMode::TimeAttack) {
        return;
    }

    if (racePage->watchedPlayerId() != lastWatchedPlayerId) {
        setVisible(false);
        return;
    }

    auto *saveManager = System::SaveManager::Instance();
    switch (saveManager->getSetting<SP::ClientSettings::Setting::TAGhostTagVisibility>()) {
    case SP::ClientSettings::TAGhostTagVisibility::None:
        setVisible(false);
        return;
    case SP::ClientSettings::TAGhostTagVisibility::Watched:
        setVisible(m_playerId == lastWatchedPlayerId);
        return;
    default:
        return;
    }
}

void CtrlRaceNameBalloon::refresh(u32 playerId) {
    if (static_cast<s32>(playerId) == m_playerId) {
        return;
    }

    m_playerId = playerId;

    auto *roomManager = SP::RoomManager::Instance();
    bool isOnline = roomManager;
    bool isRemote = roomManager && roomManager->isPlayerRemote(playerId);

    auto *pane = m_mainLayout.findPaneByName("chara_name");
    assert(pane);
    auto *material = pane->getMaterial();
    assert(material);
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    GXColor color{255, 255, 255, 255};
    if (raceScenario.spMaxTeamSize >= 2) {
        color = TeamColors::Get(raceScenario.players[m_playerId].spTeam);
    }
    material->tevColors[1] = {color.r, color.g, color.b, color.a};

    auto *saveManager = System::SaveManager::Instance();
    auto playerType = raceScenario.players[playerId].type;
    if (raceScenario.spMaxTeamSize < 2) {
        if (isOnline) {
            u32 regionLineColor = roomManager->getPlayer(playerId)->m_regionLineColor;
            color = TeamColors::Get(regionLineColor < 4 ? regionLineColor : regionLineColor ^ 1);
        } else if (playerType == System::RaceConfig::Player::Type::Local) {
            GXColor colors[4] = {
                    {232, 212, 0, 255},
                    {9, 170, 255, 255},
                    {255, 39, 127, 255},
                    {38, 220, 65, 255},
            };
            color = colors[playerId];
        }
    }
    for (size_t i = 0; i < 4; i++) {
        m_linePane->setVtxColor(i, color);
    }

    u32 characterId = raceScenario.players[playerId].characterId;
    if (playerType == System::RaceConfig::Player::Type::CPU && !isRemote) {
        setPaneVisible("chara", false);
        setMessage("chara_name", Registry::GetCharacterMessageId(characterId, true));
        return;
    }

    setPaneVisible("chara", true);

    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    if (!isOnline && playerType == System::RaceConfig::Player::Type::Local && characterId < 0x18 &&
            localPlayerCount > 1) {
        setPicture("chara", Registry::GetCharacterPane(characterId));
        setMessage("chara_name", Registry::GetCharacterMessageId(characterId, true));
        return;
    }

    auto *miiGroup = &SectionManager::Instance()->globalContext()->m_playerMiis;
    setMiiPicture("chara", miiGroup, playerId, 2);

    if (raceScenario.players[playerId].type != System::RaceConfig::Player::Type::Ghost) {
        refreshTextMiiName(playerId);
        return;
    }

    switch (saveManager->getSetting<SP::ClientSettings::Setting::TAGhostTagContent>()) {
    case SP::ClientSettings::TAGhostTagContent::Name:
        refreshTextMiiName(playerId);
        break;
    case SP::ClientSettings::TAGhostTagContent::Time:
        refreshTextTime(playerId, /* leadingZeroes */ true);
        break;
    case SP::ClientSettings::TAGhostTagContent::TimeNoLeading:
        refreshTextTime(playerId, /* leadingZeroes */ false);
        break;
    case SP::ClientSettings::TAGhostTagContent::Date:
        refreshTextDate(playerId);
        break;
    }
}

void CtrlRaceNameBalloon::refreshTextMiiName(u32 playerId) {
    MessageInfo info{};
    info.miis[0] = SectionManager::Instance()->globalContext()->m_playerMiis.get(playerId);
    setMessage("chara_name", 9501, &info);
}

void CtrlRaceNameBalloon::refreshTextTime(u32 playerId, bool leadingZeroes) {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    bool isReplay = raceScenario.players[0].type == System::RaceConfig::Player::Type::Ghost;
    u32 index = isReplay ? playerId : playerId - 1;
    auto *header = reinterpret_cast<System::RawGhostHeader *>((*raceScenario.ghostBuffer)[index]);
    MessageInfo info{};
    info.intVals[0] = header->raceTime.minutes;
    info.intVals[1] = header->raceTime.seconds;
    info.intVals[2] = header->raceTime.milliseconds;
    setMessage("chara_name", leadingZeroes ? 1400 : 10071, &info);
}

void CtrlRaceNameBalloon::refreshTextDate(u32 playerId) {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    bool isReplay = raceScenario.players[0].type == System::RaceConfig::Player::Type::Ghost;
    u32 index = isReplay ? playerId : playerId - 1;
    auto *header = reinterpret_cast<System::RawGhostHeader *>((*raceScenario.ghostBuffer)[index]);
    MessageInfo info{};
    info.intVals[0] = header->year + 2000;
    info.intVals[1] = header->month;
    info.intVals[2] = header->day;
    setMessage("chara_name", 10048, &info);
}

BalloonManager::BalloonManager() : m_namePositions(new Vec3[12]) {}

BalloonManager::~BalloonManager() = default;

void BalloonManager::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~BalloonManager();
    }
}

void BalloonManager::init(u8 localPlayerId) {
    m_localPlayerId = localPlayerId;
    for (size_t i = 0; i < std::size(m_playerIds); i++) {
        m_playerIds[i] = -1;
    }
    for (size_t i = 0; i < std::size(m_nameIsEnabled); i++) {
        m_nameIsEnabled[i] = false;
    }
}

void BalloonManager::addNameControl(CtrlRaceNameBalloon * /* nameControl */) {
    m_nameCount++;
}

void BalloonManager::calc() {
    auto *saveManager = System::SaveManager::Instance();

    SP::ClientSettings::FarPlayerTags farPlayerTags =
            saveManager->getSetting<SP::ClientSettings::Setting::FarPlayerTags>();
    float newPlayerTagRenderDistance =
            7500.0f * (farPlayerTags == SP::ClientSettings::FarPlayerTags::Disable ? 1 : 10);
    playerTagRenderDistance = newPlayerTagRenderDistance;

    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.gameMode != System::RaceConfig::GameMode::TimeAttack) {
        u32 totalNameCount = saveManager->getSetting<SP::ClientSettings::Setting::PlayerTags>();
        u32 localPlayerCount = raceScenario.localPlayerCount;
        if (localPlayerCount == 0) {
            localPlayerCount = 1;
        }
        u32 nameCount = totalNameCount / localPlayerCount;
        nameCount += m_localPlayerId < (totalNameCount % localPlayerCount);
        if (nameCount != m_nameCount) {
            for (u32 i = 0; i < m_nameCount; i++) {
                m_playerIds[i] = -1;
                m_nameIsEnabled[i] = false;
            }
            m_nameCount = nameCount;
        }
    }

    REPLACED(calc)();
}

} // namespace UI
