#pragma once

#include "MenuInputManager.h"
#include "UIControl.h"

typedef struct {
    UIControl;
    u8 _098[0x538 - 0x098];
} SheetSelectControl;

SheetSelectControl *SheetSelectControl_ct(SheetSelectControl *this);

void SheetSelectControl_dt(SheetSelectControl *this, s32 type);

// TODO r10
void SheetSelectControl_load(SheetSelectControl *this, const char *dir, const char *rightFile, const char *rightVariant, const char *leftFile, const char *leftVariant, u32 playerFlags, bool r10, bool pointerOnly);

void SheetSelectControl_setRightHandler(SheetSelectControl *this, InputHandler *handler);

void SheetSelectControl_setLeftHandler(SheetSelectControl *this, InputHandler *handler);
