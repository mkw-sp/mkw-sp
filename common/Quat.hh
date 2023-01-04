#pragma once

#include <Common.hh>

struct _RaceServerFrame_Quat;

template <typename T>
struct QuatBase {
    T x{};
    T y{};
    T z{};
    T w{};
};

template<typename T>
struct Quat : QuatBase<T> {};

template<>
struct Quat<f32> : QuatBase<f32> {
    Quat();
    Quat(f32 x, f32 y, f32 z, f32 w);
    Quat(const _RaceServerFrame_Quat &q);
    operator _RaceServerFrame_Quat() const;

    static void Inverse(const Quat<f32> &q0, const Quat<f32> &q);
    static void Slerp(const Quat<f32> &q0, const Quat<f32> &q1, Quat<f32> &q, f32 t);
};

Quat<f32> operator*(const Quat<f32> &q0, const Quat<f32> &q1);
