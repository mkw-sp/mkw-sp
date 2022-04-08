#pragma once

#include <Common.h>

// Tentative name
typedef struct {
    char _[0x54 - 0x00];
    char _54[0x88 - 0x54];
    int _88;
    int mCurrentItemKind;
    int mCurrentItemQty;
    // ...
} KartItem;

// Tentative name
typedef struct {
    char _[0x14 - 0x00];
    KartItem *mKartItems;
    // ....
} ItemDirector;
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
