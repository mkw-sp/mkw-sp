#pragma once

#include <Common.h>

void PSMTXIdentity(float mtx[3][4]);

void PSMTXMultVec(float mtx[3][4], const float src[3], float dst[3]);

void C_MTXOrtho(float mtx[4][4], float top, float bottom, float left, float right,
        float near, float far);

void PSQUATMultiply(const Quat *p, const Quat *q, Quat *pq);
void PSQUATInverse(const Quat *src, Quat *inv);

void C_QUATSlerp(const Quat *p, const Quat *q, Quat *r, f32 t);
