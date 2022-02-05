#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x90 - 0x00];
} Pad;
static_assert(sizeof(Pad) == 0x90);

void Pad_calc(Pad *this);

typedef struct {
    Pad;
    u8 _90[0xa8 - 0x90];
} GhostPad;
static_assert(sizeof(GhostPad) == 0xa8);

GhostPad *GhostPad_ct(GhostPad *this);

typedef enum {
    BUTTON_ACCEL = 1 << 0,
    BUTTON_BRAKE = 1 << 1,
    BUTTON_ITEM = 1 << 2,
    BUTTON_DRIFT = 1 << 3,
    BUTTON_UNK4 = 1 << 4,
    BUTTON_LOOK_BACKWARDS = 1 << 5,

    BUTTON_BRAKEDRIFT = BUTTON_UNK4,
} ButtonFlags;

typedef struct {
    void *_00;
    u16 buttons; // ButtonFlags
    u16 rawButtons;
    Vec2 stick;
    u8 rawStick[2];
    u8 trick;
    u8 rawTrick;
    u8 _14[0x18 - 0x14];
} RaceInputState;
static_assert(sizeof(RaceInputState) == 0x18);

typedef struct {
    u8 _00[0x28 - 0x00];
    RaceInputState currentInputState;
    RaceInputState lastInputState;
    u8 _58[0xd8 - 0x58];
} PadProxy;
static_assert(sizeof(PadProxy) == 0xd8);

// TODO r5
void PadProxy_setPad(PadProxy *this, Pad *pad, Pad *r5);

typedef struct {
    PadProxy;
    u8 _d8[0xe4 - 0xd8];
    bool isLocked;
    u8 _e5[0xec - 0xe5];
} GhostPadProxy;
static_assert(sizeof(GhostPadProxy) == 0xec);

GhostPadProxy *GhostPadProxy_ct(GhostPadProxy *this);

void GhostPadProxy_setPad(
        GhostPadProxy *this, GhostPad *pad, const void *inputs, bool driftIsAuto);

void GhostPadProxy_start(GhostPadProxy *this);

void GhostPadProxy_calc(GhostPadProxy *this, bool isPaused);

void GhostPadProxy_reset(GhostPadProxy *this);

typedef struct {
    u8 _0000[0x0004 - 0x0000];
    GhostPadProxy ghostProxies[4];
    u8 _03b4[0x1690 - 0x03b4];
    Pad dummyPad;
    u8 _1720[0x415c - 0x1720];
    GhostPad multiGhostPads[11];          // Added
    GhostPadProxy multiGhostProxies[11];  // Added
} InputManager;
static_assert(offsetof(InputManager, multiGhostPads) == 0x415c);

extern InputManager *s_inputManager;

InputManager *InputManager_createInstance(void);

InputManager *InputManager_ct(InputManager *this);

void InputManager_setGhostPad(
        InputManager *this, u32 ghostId, const void *ghostInputs, bool driftIsAuto);

void InputManager_startRace(InputManager *this);

void InputManager_startGhostProxies(InputManager *this);
