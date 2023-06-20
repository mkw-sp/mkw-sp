#include "RaceScene.hh"

#include "game/battle/CoinManager.hh"
#include "game/effect/EffectManager.hh"
#include "game/enemy/EnemyManager.hh"
#include "game/gfx/CameraManager.hh"
#include "game/item/ItemManager.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/net/NWC24Manager.hh"
#include "game/obj/ObjDirector.hh"
#include "game/race/BoxColManager.hh"
#include "game/race/DriverManager.hh"
#include "game/race/JugemManager.hh"
#include "game/system/HBMManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/ResourceManager.hh"
#include "game/ui/SectionManager.hh"

#include <features/save_states/SaveStates.hh>
#include <sp/SaveStateManager.hh>
#include <sp/cs/RaceClient.hh>

namespace Scene {

extern "C" {
bool unk_8078ddb4();

extern u32 unk_809c1874;
extern bool unk_809c19a0;
extern bool unk_809c21d8;
extern bool unk_809c2f3c;
}

void RaceScene::calcSubsystems() {
    s32 drift = 0;

    if (auto *raceClient = SP::RaceClient::Instance()) {
        raceClient->calcRead();
        /*drift = raceClient->drift();
        raceClient->adjustDrift();*/
    }

    calcSubsystems(drift);
    /*if (drift < 0) {
        calcSubsystems(0);
    }*/

    if (auto *cameraManager = Graphics::CameraManager::Instance();
            cameraManager && cameraManager->isReady()) {
        if (auto *raceClient = SP::RaceClient::Instance()) {
            raceClient->calcWrite();
        }
    }
}

void RaceScene::calcSubsystems(s32 drift) {
    if (m_isPaused) {
        unk_809c19a0 = true;
        unk_809c1874 |= 1;
        if (!unk_8078ddb4()) {
            unk_809c2f3c = true;
        }
        unk_809c21d8 = true;
    } else {
        unk_809c19a0 = false;
        unk_809c1874 &= ~1;
        if (unk_8078ddb4()) {
            unk_809c2f3c = false;
        }
        unk_809c21d8 = false;

        auto *raceManager = System::RaceManager::Instance();

        if (drift <= 0) {
            raceManager->calc();

            if (SP::RaceClient::Instance()) {
                System::InputManager::Instance()->calcRollbacks();
            }

            if (!SP::RoomManager::Instance() ||
                    raceManager->hasReachedStage(System::RaceManager::Stage::Countdown)) {
                Race::BoxColManager::Instance()->calc();
                Geo::ObjDirector::Instance()->calc();
            }

            Enemy::EnemyManager::Instance()->calc();
            Race::DriverManager::Instance()->calc();
            Kart::KartObjectManager::Instance()->calc();
            Race::JugemManager::Instance()->calc();

            if (raceManager->hasReachedStage(System::RaceManager::Stage::Countdown)) {
                Item::ItemManager::Instance()->calc();
            }

            if (!SP::RoomManager::Instance() ||
                    raceManager->hasReachedStage(System::RaceManager::Stage::Countdown)) {
                Geo::ObjDirector::Instance()->calcBT();
            }

            Effect::EffectManager::Instance()->calc();
        }

        raceManager->dynamicRandom()->nextU32();
    }

    if (!System::HBMManager::Instance()->isActive()) {
        if (drift >= 0) {
            UI::SectionManager::Instance()->calc();
            if (auto *coinManager = Battle::CoinManager::Instance()) {
                coinManager->calcScreens();
            }
        }
    }
}

void RaceScene::destroySubsystems() {
    REPLACED(destroySubsystems)();
#if ENABLE_SAVE_STATES
    SP::SaveStateManager::DestroyInstance();
#endif
}

void RaceScene::createSubsystems() {
    REPLACED(createSubsystems)();
#if ENABLE_SAVE_STATES
    SP::SaveStateManager::CreateInstance();
#endif
}

void RaceScene::loadArchives() {
    auto *config = System::RaceConfig::Instance();
    assert(config != nullptr);

    auto *resMgr = System::ResourceManager::Instance();
    assert(resMgr != nullptr);

    // Copy relevant settings from menuScenario: at this point, we should only look at the RACE
    // scenario for all future settings.
    auto gameMode = config->menuScenario().gameMode;
    if (gameMode == System::GameMode::Awards) {
        config->initRaceScenarioForAwards();
    } else if (gameMode == System::GameMode::Credits) {
        config->initRaceScenarioForCredits();
    } else {
        config->initRaceScenario();
    }

    const auto &raceScenario = config->raceScenario();

    // Create the per-player heap
    u32 playerHeapSize = 0x4B000 * raceScenario.playerCount;
    if (playerHeapSize > 0x320000) {
        playerHeapSize = 0x320000;
    }
    m_playerHeap = EGG::ExpHeap::Create(playerHeapSize, Instance()->m_rootHeapCollection.mem1, 0);

    // Attach Common.szs
    {
        auto *common = resMgr->load(System::ArcType::Race, Instance()->m_rootHeapCollection.mem1);
        registerArchive(common, System::ArcType::Race);
    }

    // Attach TRACK archive
    {
        System::MultiDvdArchive *trackFile = nullptr;
        if (raceScenario.competition) {
            Net::CompetitionInfo desc;
            if (Net::NW4C24Manager::InitStaticInstance()->getCompetitionInfo(desc)) {
                assert(desc.m_RKCT != nullptr);
                auto common_file = desc.m_RKCT->RKCO.commonFiles;
                auto course = magic_enum::enum_cast<Registry::Course>(desc.m_RKCT->RKCO.courseId);
                assert(course.has_value() && "Invalid course ID in competition file");
                trackFile = resMgr->loadCompetition(*course, &desc.m_RKCT->yazDecompSize,
                        desc.m_RKCT->header.dataLength - desc.m_RKCT->header.headersLength,
                        Instance()->m_rootHeapCollection.mem2, common_file);
            }
        } else if (gameMode == System::GameMode::Mission) {
            bool splitScreen = raceScenario.viewportCount > 1;
            trackFile = resMgr->loadMission(raceScenario.courseId, raceScenario.mrfile,
                    Instance()->m_rootHeapCollection.mem2, splitScreen);
        } else {
            bool splitScreen = raceScenario.viewportCount > 1 && !IsOnline(gameMode);
            trackFile = resMgr->loadCourse(raceScenario.courseId,
                    Instance()->m_rootHeapCollection.mem2, splitScreen);
        }
        assert(trackFile != nullptr);
        registerArchive(trackFile, System::ArcType::Course);
    }

    u32 camMode = raceScenario.cameraMode;

    // Attach DEMO archive
    {
        if (gameMode == System::GameMode::Awards) {
            if (camMode == 11 || camMode == 12) {
                auto *loseArc = resMgr->load(System::ArcType::Demo,
                        Instance()->m_rootHeapCollection.mem2, "/Demo/Lose");
                assert(loseArc != nullptr);
                registerArchive(loseArc, System::ArcType::Demo);
            } else {
                auto *awardArc = resMgr->load(System::ArcType::Demo,
                        Instance()->m_rootHeapCollection.mem2, "/Demo/Award");
                assert(awardArc != nullptr);
                registerArchive(awardArc, System::ArcType::Demo);
            }
        } else if (gameMode == System::GameMode::Credits) {
            auto *endArc = resMgr->load(System::ArcType::Demo,
                    Instance()->m_rootHeapCollection.mem2, "/Demo/Ending");
            assert(endArc != nullptr);
            registerArchive(endArc, System::ArcType::Demo);
        }
    }

    // Attach player models
    u32 playerCount = raceScenario.playerCount;
    for (u32 playerId = 0; playerId < playerCount; ++playerId) {
        EGG::Heap *archiveHeap = Instance()->m_rootHeapCollection.mem2;

        if (m_playerHeap->getAllocatableSize(4) > 0x4B000) {
            archiveHeap = m_playerHeap;
        }

        bool isTeamMode = false;
        switch (gameMode) {
        case System::GameMode::OfflineBT:
        case System::GameMode::OnlinePublicBT:
        case System::GameMode::OnlinePrivateBT:
            isTeamMode = true;
            break;
        default:
            break;
        }

        int teamId = 2;
        if (isTeamMode) {
            teamId = raceScenario.players[playerId].team;
        }

        int lodId = 0;
        bool isLocal = 0;
        {
            using PlayerType = System::RaceConfig::Player::Type;
            u32 viewportCount = raceScenario.viewportCount;
            auto playerType = raceScenario.players[playerId].type;
            if (viewportCount == 2) {
                if (playerType == PlayerType::CPU || playerType == PlayerType::Online) {
                    lodId = 2;
                } else if (playerType == PlayerType::Local) {
                    isLocal = true;
                }
            } else if (viewportCount > 2) {
                if (playerType == PlayerType::CPU) {
                    lodId = 2;
                } else if (playerType == PlayerType::Local) {
                    isLocal = true;
                }
            }
            if (camMode < 3 || camMode > 4) {
                if (camMode == 2 && raceScenario.players[playerId].screenId != -1) {
                    lodId = 0;
                }
            } else if (raceScenario.players[playerId].screenId != -1) {
                lodId = 0;
                isLocal = true;
            }
        }
        // Load kart SZS file
        {
            System::MultiDvdArchive *archive = nullptr;
            for (int attempt = 0; attempt < 5; ++attempt) {
                archive = resMgr->loadKartFromArchiveLOD(playerId,
                        raceScenario.players[playerId].vehicleId,
                        raceScenario.players[playerId].characterId, teamId, lodId, archiveHeap,
                        Instance()->m_rootHeapCollection.mem2);
                if (archive->isLoaded()) {
                    break;
                }
                archive->clear();
            }
            assert(archive != nullptr);
            registerArchive(archive, System::ArcType::Player);
        }
        // HD kart SZS File
        if (isLocal) {
            EGG::Heap *kartHeap = Instance()->m_rootHeapCollection.mem2;
            if (m_playerHeap->getAllocatableSize(4) > 0x4B000) {
                kartHeap = m_playerHeap;
            }
            System::MultiDvdArchive *archive = nullptr;
            for (int attempt = 0; attempt < 5; ++attempt) {
                archive = resMgr->loadKartFromArchiveHD(playerId,
                        raceScenario.players[playerId].vehicleId,
                        raceScenario.players[playerId].characterId, teamId, 2, kartHeap,
                        Instance()->m_rootHeapCollection.mem2);
                if (archive->isLoaded()) {
                    break;
                }
                archive->clear();
            }
            assert(archive != nullptr);
            registerArchive(archive, System::ArcType::Player);
        }
    }

    // Shrink the player heap
    m_playerHeap->adjust();
}

} // namespace Scene
