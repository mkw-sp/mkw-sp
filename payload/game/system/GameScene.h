#pragma once

#include "../host_system/Scene.h"

typedef struct {
    u8 _0000[0x0c94 - 0x0000];
    HeapCollection volatileHeapCollection;
    u8 _18a0[0x254c - 0x18a0];
} GameScene;
static_assert(sizeof(GameScene) == 0x254c);

GameScene *GameScene_get(void);
