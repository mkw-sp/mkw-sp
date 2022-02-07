#pragma once

#include <Common.h>

typedef struct {
    const struct Jugem_vt *vt;
    u8 _004[0x218 - 0x004];
} Jugem;
static_assert(sizeof(Jugem) == 0x218);

typedef struct Jugem_vt {
    u8 _00[0x08 - 0x00];
    void *vf_08;
    void *vf_0c;
    void (*calc)(Jugem *this);
} Jugem_vt;
static_assert(sizeof(Jugem_vt) == 0x14);

void Jugem_setVisible(Jugem *this, bool visible);

u32 Jugem_getPlayerId(Jugem *this);
