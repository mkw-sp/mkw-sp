#pragma once

#include <Common.h>

// Tentative name
typedef struct {
    u8 _00[0x88 - 0x00];
    s32 _88;
    s32 mCurrentItemKind;
    s32 mCurrentItemQty;
    u8 _94[0x248 - 0x94];
} KartItem;

// Tentative name
typedef struct {
    u8 _00[0x14 - 0x00];
    KartItem *m_kartItems;
    u8 _48[0x430 - 0x18];
} ItemDirector;

static_assert(sizeof(KartItem) == 0x248);
static_assert(sizeof(ItemDirector) == 0x430);
// extern ItemDirector *s_itemDirector;

// Tentative name
extern void KartKiller_calcTracking(void *self);
// The porting tool fails on the address
static ItemDirector **GetItemDirector(void) {
    const u32 ppc_addi = *(const u32 *)(((const char *)&KartKiller_calcTracking) + 0xb0);
    assert((ppc_addi >> (32 - 6)) == 15);
    assert(((ppc_addi << 6) >> (32 - 5)) == 4);
    assert(((ppc_addi << (6 + 5)) >> (32 - 5)) == 0);
    const s32 addr_hi = (s16)(ppc_addi & 0xffff);

    const u32 ppc_lwz = *(const u32 *)(((const char *)&KartKiller_calcTracking) + 0xb8);
    assert((ppc_lwz >> (32 - 6)) == 32);
    assert(((ppc_lwz << 6) >> (32 - 5)) == 4);
    assert(((ppc_lwz << (6 + 5)) >> (32 - 5)) == 4);
    const s32 addr_lo = (s16)(ppc_lwz & 0xffff);

    return (ItemDirector **)((addr_hi << 16) + addr_lo);
}

#define s_itemDirector (*GetItemDirector())
