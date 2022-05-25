#pragma once

#include <Common.h>

enum {
    INPUT_ID_FRONT = 0x0,
    INPUT_ID_BACK = 0x1,
    INPUT_ID_OPTION = 0x2,
};

struct InputHandler;

typedef struct {
    u8 _0[0x8 - 0x0];
    void (*handle)(struct InputHandler *handler, u32 localPlayerId);
} InputHandler_vt;
static_assert(sizeof(InputHandler_vt) == 0xc);

typedef struct InputHandler {
    const InputHandler_vt *vt;
} InputHandler;

typedef struct {
    u8 _00[0xf - 0x0];
} MenuInputManager;
static_assert(sizeof(MenuInputManager) == 0xf);

MenuInputManager *MenuInputManager_ct(MenuInputManager *this);

void MenuInputManager_dt(MenuInputManager *this, s32 type);

typedef struct {
    u8 _00[0x18 - 0x00];
    InputHandler *selectHandler;
    u8 _1c[0x28 - 0x1c];
} ControlInputManagerPane;
static_assert(sizeof(ControlInputManagerPane) == 0x28);

typedef struct {
    u8 _00[0x0c - 0x00];
    ControlInputManagerPane pane;
    u8 _34[0x3c - 0x34];
    struct MultiControlInputManager *parent;
    u8 _40[0x84 - 0x40];
} ControlInputManager;
static_assert(sizeof(ControlInputManager) == 0x84);

void ControlInputManager_setHandler(ControlInputManager *this, u32 inputId, InputHandler *handler,
        bool repeat);

typedef struct MultiControlInputManager {
    BASE(MenuInputManager);
    u8 _00f[0x224 - 0x00f];
} MultiControlInputManager;
static_assert(sizeof(MultiControlInputManager) == 0x224);

MultiControlInputManager *MultiControlInputManager_ct(MultiControlInputManager *this);

void MultiControlInputManager_dt(MultiControlInputManager *this, s32 type);

void MultiControlInputManager_init(MultiControlInputManager *this, u32 playerFlags,
        bool isMultiPlayer);

enum {
    WRAPPING_MODE_BOTH = 0x0,
    WRAPPING_MODE_Y = 0x1,
    WRAPPING_MODE_NEITHER = 0x2,
};

void MultiControlInputManager_setWrappingMode(MultiControlInputManager *this, u32 mode);

// TODO remaining args
void MultiControlInputManager_setHandler(MultiControlInputManager *this, u32 inputId,
        InputHandler *handler, bool r6, bool r7);

// TODO r6
void MultiControlInputManager_select(MultiControlInputManager *this, u32 localPlayerId,
        ControlInputManager *child, s32 r6);
