#include "TQuat.hh"

#include <protobuf/Race.pb.h>

Quat::Quat() = default;

Quat::Quat(f32 x, f32 y, f32 z, f32 w) : TQuatBase{x, y, z, w} {}

Quat::Quat(const _RaceServerFrame_Quat &q) : Quat(q.x, q.y, q.z, q.w) {}

Quat::operator _RaceServerFrame_Quat() const {
    return {x, y, z, w};
}
