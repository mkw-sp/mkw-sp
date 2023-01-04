#pragma once

#include <Common.hh>

struct _RaceServerFrame_Vec3;

template <typename T>
struct Vec3Base {
    T x{};
    T y{};
    T z{};
};

template<typename T>
struct Vec3 : Vec3Base<T> {};

template<>
struct Vec3<f32> : Vec3Base<f32> {
    Vec3();
    Vec3(f32 x, f32 y, f32 z);
    Vec3(const _RaceServerFrame_Vec3 &v);
    operator _RaceServerFrame_Vec3() const;

    static f32 Dot(const Vec3<f32> &v0, const Vec3<f32> &v1);
    static void ProjUnit(Vec3<f32> &v, const Vec3<f32> &v0, const Vec3<f32> &v1);
    static f32 Norm(const Vec3<f32> &v);
};

Vec3<f32> operator+(const Vec3<f32> &v0, const Vec3<f32> &v1);
Vec3<f32> operator-(const Vec3<f32> &v0, const Vec3<f32> &v1);

Vec3<f32> operator*(const f32 &s, const Vec3<f32> &v0);

Vec3<f32> &operator+=(Vec3<f32> &v, const Vec3<f32> &v0);
Vec3<f32> &operator-=(Vec3<f32> &v, const Vec3<f32> &v0);
