#include "ObjPylon01.hh"

#include "game/system/RaceConfig.hh"

namespace Geo {

PixelMode ObjPylon01::pixelMode() const {
    const auto &player = System::RaceConfig::Instance()->raceScenario().players[0];
    bool ghost = player.type != System::RaceConfig::Player::Type::Ghost && m_playerId != 0;
    return ghost ? PixelMode::XLU : PixelMode::OPA;
}

} // namespace Geo
