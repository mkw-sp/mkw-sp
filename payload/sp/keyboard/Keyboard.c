#include <Common.h>
#include <revolution.h>

#include <sp/TypingBuffer.h>

#include "Keyboard.h"
#include "SIKeyboard.h"

typedef enum {
    kInputDeviceNone,
    kInputDeviceSI,
} InputDevice;

// Dolphin can detect up background input if configured to do so
// Mashing the keyboard will rarely yield 1, extra rarely 2 events
// But background typing can be in the hundreds
// Let's only tolerate 2 buffered events
enum { MAX_BUFFERED_EVENTS = 2 };

typedef struct SimpleEvents {
    u16 events[MAX_BUFFERED_EVENTS * 6];
    size_t num_events;
} SimpleEvents;

// SIKeyboard is polled via background handler on PPC
static void SimpleEvents_ReadSI(SimpleEvents *events) {
    assert(events != NULL);

    char raw_events[ARRAY_SIZE(events->events)];
    memset(raw_events, 0, sizeof(raw_events));
    events->num_events = SIKeyboard_ConsumeBuffer(raw_events, ARRAY_SIZE(events->events));

    // Convert in-place
    bool shiftState = false;
    bool ctrlState = false;
    size_t outIndex = 0;
    for (size_t i = 0; i < events->num_events; ++i) {
        u16 ch = '\0';
        switch (raw_events[i]) {
        case SIKEY_ENTER:
            ch = kSimpleEvent_Enter;
            break;
        case SIKEY_BACKSPACE:
            ch = kSimpleEvent_Backspace;
            break;
        case SIKEY_TAB:
            ch = kSimpleEvent_Tab;
            break;
        case SIKEY_ESC:
            ch = kSimpleEvent_Escape;
            break;
        case SIKEY_LEFTARROW:
            ch = kSimpleEvent_ArrowL;
            break;
        case SIKEY_RIGHTARROW:
            ch = kSimpleEvent_ArrowR;
            break;
        case SIKEY_DOWNARROW:
            ch = kSimpleEvent_ArrowD;
            break;
        case SIKEY_UPARROW:
            ch = kSimpleEvent_ArrowU;
            break;
        case SIKEY_NEXT_IS_SHIFTED:
            shiftState = true;
            break;
        case SIKEY_LEFTCONTROL:
            ctrlState = true;
            break;
        default:
            ch = SIKeyboard_KeycodeToCharacter(raw_events[i], shiftState);
            shiftState = false;
            break;
        }
        if ((ch & 0xff) == 0) {
            continue;
        }
        if (ctrlState) {
            ch |= (kSimpleMods_CTRL << 8);
            ctrlState = false;
        }
        events->events[outIndex++] = ch;
    }
    events->num_events = outIndex;
}

typedef struct ConsoleInput {
    TypingBuffer mTypingBuffer;
    SP_LineCallback mCallback;
    SP_KeypressCallback mUnhandledKeypressCallback;
    void *mUnhandledKeypressUser;
    bool mIsConsoleOpen : 1;
    InputDevice mInputDevice : 2;
} ConsoleInput;

static bool ConsoleInput_Open(ConsoleInput *input) {
    input->mInputDevice = kInputDeviceNone;

    TypingBuffer_Init(&input->mTypingBuffer);
    input->mCallback = NULL;
    input->mIsConsoleOpen = false;

    const s32 si_keyboard = SIKeyboard_GetCurrentConnection();
    if (si_keyboard >= 0) {
        input->mInputDevice = kInputDeviceSI;
        return true;
    }

    return false;
}
static void ConsoleInput_Close(ConsoleInput * /* input */) {}

// The user closed the console window
static void ConsoleInput_EndInteraction(ConsoleInput *input) {
    input->mIsConsoleOpen = false;
    TypingBuffer_Init(&input->mTypingBuffer);
}
static void ConsoleInput_Process(ConsoleInput *input) {
    SimpleEvents events = (SimpleEvents){
            .num_events = 0,
    };

    switch (input->mInputDevice) {
    case kInputDeviceNone:
        return;
    case kInputDeviceSI:
        SimpleEvents_ReadSI(&events);
        break;
    }

    for (size_t i = 0; i < events.num_events; ++i) {
        u16 ev = events.events[i];

        // Pressing the slash key opens the console
        if (!input->mIsConsoleOpen && ev == (u16)'/') {
            input->mIsConsoleOpen = true;
        }

        // No interaction: do nothing
        if (!input->mIsConsoleOpen) {
            if (input->mUnhandledKeypressCallback != NULL) {
                input->mUnhandledKeypressCallback(ev & 0xff, ev >> 8,
                        input->mUnhandledKeypressUser);
            }
            continue;
        }

        switch (ev) {
        case kSimpleEvent_Enter:
            // Notify subscribers. Don't submit a blank line
            if (input->mTypingBuffer.len > 0 && input->mCallback != NULL) {
                (*input->mCallback)(input->mTypingBuffer.buf, input->mTypingBuffer.len);
            }

            ConsoleInput_EndInteraction(input);
            break;
        case kSimpleEvent_Backspace:
            TypingBuffer_Backspace(&input->mTypingBuffer);
            break;
        case kSimpleEvent_Escape:
        case kSimpleEvent_Tab: // Escape triggers Dolphin's "Exit Emulation" prompt,
                               // so we also accept TAB. Ideally TAB would be for
                               // auto-completion, though.
            ConsoleInput_EndInteraction(input);
            break;
        default:
            TypingBuffer_Append(&input->mTypingBuffer, (char)ev);
            break;
        }
    }
}

static bool sConsoleInput_Ready;
static ConsoleInput sConsoleInput;

bool SP_InitConsoleInput(void) {
    if (!sConsoleInput_Ready) {
        sConsoleInput_Ready = ConsoleInput_Open(&sConsoleInput);
    }

    return sConsoleInput_Ready;
}

bool SP_IsConsoleInputInit(void) {
    return sConsoleInput_Ready;
}

void SP_DestroyConsoleInput(void) {
    if (sConsoleInput_Ready) {
        ConsoleInput_Close(&sConsoleInput);
        sConsoleInput_Ready = false;
    }
}

void SP_ProcessConsoleInput(void) {
    assert(sConsoleInput_Ready);
    ConsoleInput_Process(&sConsoleInput);
}

void SP_SetLineCallback(SP_LineCallback callback) {
    assert(sConsoleInput_Ready);
    sConsoleInput.mCallback = callback;
}

void SP_SetKeypressCallback(SP_KeypressCallback callback, void *userdata) {
    assert(sConsoleInput_Ready);
    sConsoleInput.mUnhandledKeypressCallback = callback;
    sConsoleInput.mUnhandledKeypressUser = userdata;
}

SP_Line SP_GetCurrentLine(void) {
    assert(sConsoleInput_Ready);

    const SP_Line line = {.buf = sConsoleInput.mTypingBuffer.buf,
            .len = sConsoleInput.mTypingBuffer.len};

    return line;
}
bool SP_IsTyping(void) {
    assert(sConsoleInput_Ready);
    return sConsoleInput.mIsConsoleOpen;
}
