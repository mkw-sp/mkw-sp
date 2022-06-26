#include "exi.h"

#include <Common.h>

typedef struct {
    u8 _00[0x20 - 0x00];
    u32 _20;
    u8 _24[0x40 - 0x24];
} EXIControl;
static_assert(sizeof(EXIControl) == 0x40);

extern EXIControl Ecb[];

BOOL __EXIProbe(s32 chan);

void EXIProbeReset(void) {
    *(u32 *)0x800030c0 = 0;
    *(u32 *)0x800030c4 = 0;
    Ecb[0]._20 = 0;
    Ecb[1]._20 = 0;
    __EXIProbe(0);
    __EXIProbe(1);
}
