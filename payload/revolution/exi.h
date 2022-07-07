#pragma once

#include <Common.h>

enum {
    EXI_READ = 0,
    EXI_WRITE = 1,
};

BOOL EXISelect(s32 chan, u32 dev, u32 freq);
BOOL EXIDeselect(s32 chan);
void EXIProbeReset(void);
BOOL EXIImm(s32 chan, void *buf, s32 len, u32 type, void *callback);
BOOL EXISync(s32 chan);

BOOL EXILock(s32 chan, u32 dev, void *unlockedCallback);
BOOL EXIUnlock(s32 chan);
