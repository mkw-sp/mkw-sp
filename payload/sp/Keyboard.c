#include <Common.h>
#include <revolution.h>

#include "IOSKeyboard.h"
#include "Keyboard.h"
#include "SIKeyboard.h"
#include "TypingBuffer.h"

typedef enum {
    kInputDeviceNone,
    kInputDeviceIOS,
    kInputDeviceSI,
} InputDevice;

// Other values are characters
typedef enum SimpleEvent {
    kSimpleEvent_Enter = 0xff,
    kSimpleEvent_Backspace = 0xfe,
    kSimpleEvent_Escape = 0xfd,
    kSimpleEvent_Tab = 0xfc,
} SimpleEvent;

// Dolphin can detect up background input if configured to do so
// Mashing the keyboard will rarely yield 1, extra rarely 2 events
// But background typing can be in the hundreds
// Let's only tolerate 2 buffered events
enum { MAX_BUFFERED_EVENTS = 2 };

typedef struct SimpleEvents {
    char events[MAX_BUFFERED_EVENTS * 6];
    size_t num_events;
} SimpleEvents;

static void ReadSimpleEvents(IOSKeyboard keyboard, SimpleEvents *simpleEvents) {
    IOSKeyboard_Event events[MAX_BUFFERED_EVENTS];
    size_t num_events = IOSKeyboard_PollBuffered(keyboard, events, MAX_BUFFERED_EVENTS);

    int num_simple_events = 0;

    for (size_t i = 0; i < num_events; ++i) {
        IOSKeyboard_Event *ev = &events[i];

        if (ev->message != kKeyboardMessage_Press)
            continue;

        const bool shift_pressed = ev->modifiers.left_shift | ev->modifiers.right_shift;

        for (int j = 0; j < 6; ++j) {
            const IOSKeyboard_KeyCode pressed = (IOSKeyboard_KeyCode)ev->pressed[j];

            if (pressed == 0)
                continue;

            char *result = &simpleEvents->events[num_simple_events++];

            switch (pressed) {
            case IOS_KEY_ENTER:
                *result = kSimpleEvent_Enter;
                break;
            case IOS_KEY_BACKSPACE:
                *result = kSimpleEvent_Backspace;
                break;
            case IOS_KEY_ESCAPE:
                *result = kSimpleEvent_Escape;
                break;
            case IOS_KEY_TAB:
                *result = kSimpleEvent_Tab;
                break;
            default:
                if (IOSKeyboard_KeycodeIsCharacter(pressed)) {
                    *result = IOSKeyboard_KeycodeToCharacter(pressed, shift_pressed);
                }
                // Ignore non-printable characters
                break;
            }
        }
    }

    simpleEvents->num_events = num_simple_events;
}

typedef struct ConsoleInput {
    TypingBuffer mTypingBuffer;
    IOSKeyboard mKeyboard;
    SP_LineCallback mCallback;
    bool mIsConsoleOpen : 1;
    InputDevice mInputDevice : 2;
} ConsoleInput;

static bool ConsoleInput_Open(ConsoleInput *input) {
    input->mInputDevice = kInputDeviceNone;
    input->mKeyboard = -1;  // 0 is a valid handle

    TypingBuffer_Init(&input->mTypingBuffer);
    input->mCallback = NULL;
    input->mIsConsoleOpen = false;

    const IOSKeyboard ios_keyboard = IOSKeyboard_Open();
    if (ios_keyboard >= 0) {
        input->mInputDevice = kInputDeviceIOS;
        input->mKeyboard = ios_keyboard;
        return true;
    }

    const s32 si_keyboard = SIKeyboard_GetCurrentConnection();
    if (si_keyboard >= 0) {
        input->mInputDevice = kInputDeviceSI;
        return true;
    }

    return false;
}
static void ConsoleInput_Close(ConsoleInput *input) {
    IOSKeyboard_Close(input->mKeyboard);
    input->mKeyboard = -1;
}

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
    case kInputDeviceIOS:
        ReadSimpleEvents(input->mKeyboard, &events);
        break;
    case kInputDeviceSI:
        // SIKeyboard is polled via background handler
        events.num_events =
                SIKeyboard_ConsumeBuffer(events.events, sizeof(events.events));
        break;
    }

    for (size_t i = 0; i < events.num_events; ++i) {
        SimpleEvent ev = events.events[i];

        // Pressing the slash key opens the console
        if (!input->mIsConsoleOpen && (char)ev == '/') {
            input->mIsConsoleOpen = true;
        }

        // No interaction: do nothing
        if (!input->mIsConsoleOpen)
            continue;

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
        case kSimpleEvent_Tab:  // Escape triggers Dolphin's "Exit Emulation" prompt,
                                // so we also accept TAB. Ideally TAB would be for
                                // auto-completion, though.
            ConsoleInput_EndInteraction(input);
            break;
        default:
            TypingBuffer_Append(&input->mTypingBuffer, (char)ev);

            SP_LOG("BUFFER %s", &input->mTypingBuffer.buf);
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

SP_Line SP_GetCurrentLine(void) {
    assert(sConsoleInput_Ready);

    const SP_Line line = { .buf = sConsoleInput.mTypingBuffer.buf,
        .len = sConsoleInput.mTypingBuffer.len };

    return line;
}
