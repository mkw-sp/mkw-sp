#pragma once

#include "Obj.h"

typedef struct {
    Obj parent;
    u8 _0b0[0x108 - 0x0b0];
    u32 playerId;
    u8 _10c[0x118 - 0x10c];
} ObjPylon01;
static_assert(sizeof(ObjPylon01) == 0x118);

REPLACE u32 ObjPylon01_vf_b0(const ObjPylon01 *self);
