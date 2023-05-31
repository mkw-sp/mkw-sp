#include "IOSKeyboard.h"
#include <revolution.h> // OSReport
#include <revolution/ios.h>
#include <string.h> // memset

// #ifdef DEBUG
#define LOG OSReport
// #else
// #define LOG(...)
// #endif

static const char keys[] = {
        0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
        'z', // 29
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        0,   // 40, Enter
        0,   // 41, escape
        0,   // 42, backspace
        0,   // 43, tab
        ' ', // 44 space
        '-', // 45
        '=', // 46
        '[', ']',
        '?',  // unknown
        '\\', // right \ PAD
        ';', '\'', '~', ',', '.', '/',
        0 // CAPS = 57
};
static const char keys_shift[] = {
        0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
        'Z', // 29
        '!', // 31 -- This is wrong on WiiBrew, actually 30
        '@', // 32
        '#', '$', '%', '^', '&', '*', '(', ')',
        0,   // 40, Enter
        0,   // 41, escape
        0,   // 42, backspace
        0,   // 43, tab
        ' ', // 44 space
        '_', // 45
        '+', // 46
        '{', '}',
        '?', // unknown
        '|', // right \ PAD
        ':', '"',
        '~', // TODO
        '<', '>', '?',
        0 // CAPS = 57
};

void IOSKeyboard_DumpEvent(const IOSKeyboard_Event *ev) {
    static const char *const messages[3] = {"Keyboard connect", "Keyboard disconnect", "Key press"};

    OSReport("MessageType: %u %s\n", ev->message,
            ev->message < 3 ? messages[ev->message] : "INVALID");
    OSReport("Unknown: %s\n", messages[ev->unknown]);
    OSReport("Modifiers:\n");
    if (ev->modifiers.left_control) {
        OSReport("- Left control\n");
    }
    if (ev->modifiers.left_shift) {
        OSReport("- Left shift\n");
    }
    if (ev->modifiers.left_alt) {
        OSReport("- Left alt\n");
    }
    if (ev->modifiers.left_win) {
        OSReport("- Left win\n");
    }
    if (ev->modifiers.right_control) {
        OSReport("- Right control\n");
    }
    if (ev->modifiers.right_shift) {
        OSReport("- Right shift\n");
    }
    if (ev->modifiers.right_alt) {
        OSReport("- Right alt\n");
    }
    if (ev->modifiers.right_win) {
        OSReport("- Right win\n");
    }
    OSReport("_09: %u\n", (unsigned)ev->_09);
    OSReport("Pressed:\n");
    for (int i = 0; i < 6; ++i) {
        OSReport("[%u] = %u\n", (unsigned)i, ev->pressed[i]);
    }
}

bool IOSKeyboard_KeycodeIsCharacter(IOSKeyboard_KeyCode key) {
    return key < sizeof(keys) && keys[key] != 0;
}
char IOSKeyboard_KeycodeToCharacter(IOSKeyboard_KeyCode key, bool shift) {
    if (key >= sizeof(keys)) {
        return '?';
    }

    const char *table = (shift) ? &keys_shift[0] : &keys[0];
    return table[key];
}

IOSKeyboard IOSKeyboard_Open(void) {
    const s32 fd = IOS_Open("/dev/usb/kbd", 0);

    if (fd < 0) {
        OSReport("[IOSKeyboard] Failed to open /dev/usb/kbd: Returned error %i\n", fd);
    } else {
        OSReport("[IOSKeyboard] Successfully created keyboard: ID: %i\n", fd);
    }

    return fd;
}

void IOSKeyboard_Close(IOSKeyboard keyboard) {
    if (keyboard >= 0) {
        IOS_Close(keyboard);
    }
}

s32 IOSKeyboard_PollBlocking(IOSKeyboard keyboard, IOSKeyboard_Event *ev) {
    assert(keyboard >= 0);
    assert(ev != NULL);

    // No request type?
    memset(ev, 0, sizeof(*ev));
    return IOS_Ioctl(keyboard, 0, NULL, 0, ev, sizeof(*ev));
}

size_t IOSKeyboard_PollBuffered(IOSKeyboard keyboard, IOSKeyboard_Event *events, size_t numEvents) {
    size_t i = 0;

    if (numEvents <= 0) {
        return 0;
    }

    while (IOSKeyboard_NextEvent(keyboard, &events[i])) {
        // OSReport("Event %i\n", i);
        if (i + 1 >= numEvents) {
            // - Discard the previous two events
            // - Discard all future events clearing the stack
            LOG("Detected background input:\n");

            while (IOSKeyboard_NextEvent(keyboard, &events[0]) && i < 5) {
                ++i;
            }

            LOG("-> %i keyboard packets; %i is max\n", i, numEvents);

            return 0;
        }

        ++i;
    }

    return i;
}
