#pragma once

#include "Obj.h"

typedef struct {
    Obj *objs[12 * 30];
    u32 count;
} ObjGroup;
