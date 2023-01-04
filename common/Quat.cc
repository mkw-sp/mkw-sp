#include "Quat.hh"

#include <protobuf/Race.pb.h>

Quat<f32>::Quat() = default;

Quat<f32>::Quat(f32 x, f32 y, f32 z, f32 w) : QuatBase{x, y, z, w} {}

Quat<f32>::Quat(const _RaceServerFrame_Quat &q) : Quat(q.x, q.y, q.z, q.w) {}

Quat<f32>::operator _RaceServerFrame_Quat() const {
    return {x, y, z, w};
}
