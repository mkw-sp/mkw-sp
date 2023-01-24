#include "TVec3.hh"

#include <protobuf/Race.pb.h>

Vec3::Vec3() = default;

Vec3::Vec3(f32 x, f32 y, f32 z) : TVec3Base{x, y, z} {}

Vec3::Vec3(const _PlayerFrame_Vec3 &v) : Vec3(v.x, v.y, v.z) {}

Vec3::operator _PlayerFrame_Vec3() const {
    return {x, y, z};
}

void Vec3::ProjUnit(const Vec3 &v0, const Vec3 &v1, Vec3 &v) {
    f32 dot = v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
    v = dot * v1;
}

Vec3 operator*(const f32 &s, const Vec3 &v0) {
    return {s * v0.x, s * v0.y, s * v0.z};
}

Vec3 &operator+=(Vec3 &v, const Vec3 &v0) {
    v.x += v0.x;
    v.y += v0.y;
    v.z += v0.z;
    return v;
}

Vec3 &operator-=(Vec3 &v, const Vec3 &v0) {
    v.x -= v0.x;
    v.y -= v0.y;
    v.z -= v0.z;
    return v;
}
