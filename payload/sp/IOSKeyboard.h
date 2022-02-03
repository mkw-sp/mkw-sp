#pragma once

#include <Common.h>

typedef enum {
    kKeyboardMessage_Connect,
    kKeyboardMessage_Disconnect,
    kKeyboardMessage_Press
} IOSKeyboard_Message;

typedef struct {
    bool right_win : 1;
    bool right_alt : 1;
    bool right_shift : 1;
    bool right_control : 1;
    bool left_win : 1;
    bool left_alt : 1;
    bool left_shift : 1;
    bool left_control : 1;
} IOSKeyboard_Modifiers;

typedef enum {
    IOS_KEY_A = 4,  // in order
    IOS_KEY_Z = IOS_KEY_A + 25,
    IOS_KEY_ENTER = 40,
    IOS_KEY_ESCAPE = 41,
    IOS_KEY_BACKSPACE = 42,
    IOS_KEY_TAB = 43,
    IOS_KEY_SPACE = 44,
} IOSKeyboard_KeyCode;

bool IOSKeyboard_KeycodeIsCharacter(IOSKeyboard_KeyCode key);
char IOSKeyboard_KeycodeToCharacter(IOSKeyboard_KeyCode key, bool shift);

typedef struct {
    IOSKeyboard_Message message;  // 4byte
    u32 unknown;                  // perhaps keyboard id?
    IOSKeyboard_Modifiers modifiers;
    u8 _09;
    u8 pressed[6];  // Value of IOSKeyboard_KeyCode
} IOSKeyboard_Event;

void IOSKeyboard_DumpEvent(const IOSKeyboard_Event *ev);

// Return
typedef s32 IOSKeyboard;

// Return negative value if invalid
IOSKeyboard IOSKeyboard_Open(void);
void IOSKeyboard_Close(IOSKeyboard keyboard);

// Returns IOSErr
s32 IOSKeyboard_PollBlocking(IOSKeyboard keyboard, IOSKeyboard_Event *ev);

inline bool IOSKeyboard_NextEvent(IOSKeyboard keyboard, IOSKeyboard_Event *ev) {
    IOSKeyboard_PollBlocking(keyboard, ev);
    return ev->message != 0;
}

// NOTE: This is a blocking operation, though you won't be waiting for keypresses. When
// Dolphin runs out of events to send down, it will send down a NULL event. When we
// encouter that, we'll immediately quit.
//
// If dolphin has more than numEvents queued, we assume it's background input and ignore
// it. Mashing the keyboard produces about 1-2 events per frame. Background input can
// result in hundreds.
//
// Return the number of events actually available.
size_t IOSKeyboard_PollBuffered(
        IOSKeyboard keyboard, IOSKeyboard_Event *events, size_t numEvents);