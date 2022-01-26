#pragma once

#include "MenuInputManager.h"
#include "UIControl.h"

struct SheetSelectControl;

struct SheetSelectControlHandler;

typedef struct {
    u8 _0[0x8 - 0x0];
    void (*handle)(struct SheetSelectControlHandler *handler, struct SheetSelectControl *control,
            u32 localPlayerId);
} SheetSelectControlHandler_vt;

typedef struct SheetSelectControlHandler {
    const SheetSelectControlHandler_vt *vt;
} SheetSelectControlHandler;

typedef struct SheetSelectControl {
    UIControl;
    u8 _098[0x538 - 0x098];
} SheetSelectControl;
static_assert(sizeof(SheetSelectControl) == 0x538);

SheetSelectControl *SheetSelectControl_ct(SheetSelectControl *this);

void SheetSelectControl_dt(SheetSelectControl *this, s32 type);

// TODO r10
void SheetSelectControl_load(SheetSelectControl *this, const char *dir, const char *rightFile,
        const char *rightVariant, const char *leftFile, const char *leftVariant, u32 playerFlags,
        bool r10, bool pointerOnly);

void SheetSelectControl_setRightHandler(SheetSelectControl *this,
        SheetSelectControlHandler *handler);

void SheetSelectControl_setLeftHandler(SheetSelectControl *this,
        SheetSelectControlHandler *handler);

void SheetSelectControl_setPlayerFlags(SheetSelectControl *this, u32 playerFlags);
