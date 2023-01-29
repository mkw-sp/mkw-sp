#include "TQuat.hh"

#include <protobuf/Room.pb.h>

Quat::Quat() : TQuatBase{0.0f, 0.0f, 0.0f, 1.0f} {}

Quat::Quat(f32 x, f32 y, f32 z, f32 w) : TQuatBase{x, y, z, w} {}

Quat::Quat(const _PlayerFrame_Quat &q) : Quat(q.x, q.y, q.z, q.w) {}

Quat::operator _PlayerFrame_Quat() const {
    return {x, y, z, w};
}
