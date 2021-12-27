#include "InputManager.h"

static InputManager *my_InputManager_createInstance(void) {
    s_inputManager = new(sizeof(InputManager));
    InputManager_ct(s_inputManager);

    for (u32 i = 0; i < ARRAY_SIZE(s_inputManager->multiGhostPads); i++) {
        GhostPad_ct(&s_inputManager->multiGhostPads[i]);
    }
    for (u32 i = 0; i < ARRAY_SIZE(s_inputManager->multiGhostProxies); i++) {
        GhostPadProxy_ct(&s_inputManager->multiGhostProxies[i]);
        PadProxy_setPad(&s_inputManager->multiGhostProxies[i], &s_inputManager->dummyPad, NULL);
    }

    return s_inputManager;
}

PATCH_B(InputManager_createInstance, my_InputManager_createInstance);

void InputManager_resetMultiGhostProxies(InputManager *this) {
    for (u32 i = 0; i < ARRAY_SIZE(this->multiGhostProxies); i++) {
        GhostPadProxy_reset(&this->multiGhostProxies[i]);
    }
}

void InputManager_calcMultiGhostPads(InputManager *this) {
    for (u32 i = 0; i < ARRAY_SIZE(this->multiGhostPads); i++) {
        Pad_calc(&this->multiGhostPads[i]);
    }
}

void InputManager_calcMultiGhostProxies(InputManager *this, bool isPaused) {
    for (u32 i = 0; i < ARRAY_SIZE(this->multiGhostProxies); i++) {
        GhostPadProxy_calc(&this->multiGhostProxies[i], isPaused);
    }
}

void InputManager_setGhostPad(InputManager *this, u32 ghostId, const void *ghostInputs, bool driftIsAuto) {
    GhostPadProxy *proxy = &this->multiGhostProxies[ghostId];
    GhostPad *pad = &this->multiGhostPads[ghostId];
    GhostPadProxy_setPad(proxy, pad, ghostInputs, driftIsAuto);
}

static void my_InputManager_startRace(InputManager *this) {
    for (u32 i = 0; i < ARRAY_SIZE(this->ghostProxies); i++) {
        this->ghostProxies[i].isLocked = true;
    }

    for (u32 i = 0; i < ARRAY_SIZE(this->multiGhostProxies); i++) {
        this->multiGhostProxies[i].isLocked = true;
    }
}

PATCH_B(InputManager_startRace, my_InputManager_startRace);

static void my_InputManager_startGhostProxies(InputManager *this) {
    for (u32 i = 0; i < ARRAY_SIZE(this->ghostProxies); i++) {
        GhostPadProxy_start(&this->ghostProxies[i]);
    }

    for (u32 i = 0; i < ARRAY_SIZE(this->multiGhostProxies); i++) {
        GhostPadProxy_start(&this->multiGhostProxies[i]);
    }
}

PATCH_B(InputManager_startGhostProxies, my_InputManager_startGhostProxies);
