#include "ObjPylon01.hh"

#include "game/system/RaceConfig.hh"

namespace Geo {

Obj::PixelMode ObjPylon01::vf_b0() {
    using Type = System::RaceConfig::Player::Type;
    const auto &player = System::RaceConfig::Instance()->raceScenario().players[0];
    if (player.type != Type::Ghost && m_playerId != 0) {
        return PixelMode::Transparent;
    }
    return PixelMode::Opaque;
}

} // namespace Geo
