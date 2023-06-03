#include "ObjPylon01.hh"

#include "game/system/RaceConfig.hh"

namespace Geo {

Obj::PixelMode ObjPylon01::vf_b0() {
    using Mode = PixelMode;
    using Type = System::RaceConfig::Player::Type;

    const auto &player = System::RaceConfig::Instance()->raceScenario().players[0];
    return player.type != Type::Ghost && m_playerId != 0 ? Mode::Transparent : Mode::Opaque;
}

} // namespace Geo
