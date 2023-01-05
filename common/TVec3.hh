#pragma once

#include <Common.hh>

struct _RaceServerFrame_Vec3;

template <typename T>
struct TVec3Base {
    T x{};
    T y{};
    T z{};
};

template<typename T>
struct TVec3 : TVec3Base<T> {};

struct Vec3 : TVec3Base<f32> {
    Vec3();
    Vec3(f32 x, f32 y, f32 z);
    Vec3(const _RaceServerFrame_Vec3 &v);
    operator _RaceServerFrame_Vec3() const;

    static f32 Dot(const Vec3 &v0, const Vec3 &v1);
    static void ProjUnit(const Vec3 &v0, const Vec3 &v1, Vec3 &v);
    static f32 Norm(const Vec3 &v);
};

Vec3 operator+(const Vec3 &v0, const Vec3 &v1);
Vec3 operator-(const Vec3 &v0, const Vec3 &v1);

Vec3 operator*(const f32 &s, const Vec3 &v0);

Vec3 &operator+=(Vec3 &v, const Vec3 &v0);
Vec3 &operator-=(Vec3 &v, const Vec3 &v0);

inline Vec3 cross(Vec3 lhs, Vec3 rhs) {
    return {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x,
    };
}
inline f32 dot(Vec3 lhs, Vec3 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
inline Vec3 operator*(Vec3 lhs, f32 rhs) {
    return {
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs,
    };
}
