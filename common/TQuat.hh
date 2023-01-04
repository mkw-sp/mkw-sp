#pragma once

#include <Common.hh>

struct Vec3;

struct _RaceServerFrame_Quat;

template <typename T>
struct TQuatBase {
    T x;
    T y;
    T z;
    T w;
};

template<typename T>
struct TQuat : TQuatBase<T> {};

struct Quat : TQuatBase<f32> {
    Quat();
    Quat(f32 x, f32 y, f32 z, f32 w);
    Quat(const _RaceServerFrame_Quat &q);
    operator _RaceServerFrame_Quat() const;

    static void Inverse(const Quat &q0, const Quat &q);
    static void Slerp(const Quat &q0, const Quat &q1, Quat &q, f32 t);
    static void Rotate(const Quat &q0, const Vec3 &v0, Vec3 &v);
};

Quat operator*(const Quat &q0, const Quat &q1);
