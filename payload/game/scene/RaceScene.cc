#include "RaceScene.hh"

#include <sp/cs/RaceClient.hh>
#include <sp/cs/RaceServer.hh>

namespace Scene {

void RaceScene::process() {
    if (auto *raceClient = SP::RaceClient::Instance()) {
        raceClient->calcRead();
    }

    REPLACED(process)();

    if (auto *raceServer = SP::RaceServer::Instance()) {
        raceServer->calcWrite();
    }
}

} // namespace Scene
