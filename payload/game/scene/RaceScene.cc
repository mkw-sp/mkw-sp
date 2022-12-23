#include "RaceScene.hh"

#include "game/battle/CoinManager.hh"
#include "game/effect/EffectManager.hh"
#include "game/enemy/EnemyManager.hh"
#include "game/item/ItemManager.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/obj/ObjDirector.hh"
#include "game/race/BoxColManager.hh"
#include "game/race/DriverManager.hh"
#include "game/race/JugemManager.hh"
#include "game/system/HBMManager.hh"
#include "game/system/RaceManager.hh"
#include "game/ui/SectionManager.hh"

#include <sp/cs/RaceClient.hh>
#include <sp/cs/RaceServer.hh>

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
        drift = raceClient->drift();
        raceClient->adjustDrift();
    }

    if (drift > 0 && m_frameId & 1) {
        calcSubsystems(0);
    } else {
        calcSubsystems(drift);
    }
    if (drift < 0) {
        calcSubsystems(0);
    }

    if (auto *raceServer = SP::RaceServer::Instance()) {
        raceServer->calcWrite();
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

            if (!SP::RoomManager::Instance() ||
                    raceManager->hasReachedStage(System::RaceManager::Stage::Countdown)) {
                Race::BoxColManager::Instance()->calc();
                Geo::ObjDirector::Instance()->calc();
            }
        }

        if (drift >= 0) {
            Enemy::EnemyManager::Instance()->calc();
            Race::DriverManager::Instance()->calc();
            Kart::KartObjectManager::Instance()->calc();
            Race::JugemManager::Instance()->calc();

            if (raceManager->hasReachedStage(System::RaceManager::Stage::Countdown)) {
                Item::ItemManager::Instance()->calc();
            }
        }

        if (drift <= 0) {
            if (!SP::RoomManager::Instance() ||
                    raceManager->hasReachedStage(System::RaceManager::Stage::Countdown)) {
                Geo::ObjDirector::Instance()->calcBT();
            }
        }

        if (drift >= 0) {
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

} // namespace Scene
