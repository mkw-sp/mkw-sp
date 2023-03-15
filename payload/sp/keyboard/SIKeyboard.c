// clang-format off
//
// Resources:
// - https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/HW/SI/SI_DeviceKeyboard.cpp
//   - (Initial) https://github.com/dolphin-emu/dolphin/pull/1893
// - https://github.com/DeltaResero/GC-Wii-Linux-Kernels/blob/GC-Wii-Linux-Kernel-2.6.12.y-gcLinux/drivers/input/gcn-si.c
// - http://blarg.ca/2021/02/15/using-a-gamecube-ascii-keyboard-controller-with-libogc/
// - http://hitmen.c02.at/files/yagcd/yagcd/chap9.html#sec9.3
//   - https://github.com/gered/gc-ascii-keyboard/tree/master/src
// - Phantasy Star Online 2 (official driver)
//
// clang-format on

#include "SIKeyboard.h"

#include <revolution.h>
#include <revolution/si.h>
#include <sp/ScopeLock.h>
#include <sp/TypingBuffer.h>

// Behaves like std::array<char, 3>
typedef struct {
    char keys[3];
} KeyCodeTriplet;

static bool KeyCodeTriplet_IsEmpty(const KeyCodeTriplet *trip) {
    return trip->keys[0] == 0 && trip->keys[1] == 0 && trip->keys[2] == 0;
}
static bool KeyCodeTriplet_Equals(const KeyCodeTriplet *self, const KeyCodeTriplet *other) {
    return !memcmp(self, other, sizeof(KeyCodeTriplet));
}

static s32 sSIChannel = -1;

typedef enum {
    POLL_RESULT_BUSY,
    POLL_RESULT_RECONNECT,
    POLL_RESULT_OK,
} PollResult;

static PollResult SIKeyboard_PollInternal(KeyCodeTriplet *pKeys) {
    if (sSIChannel < 0) {
        return POLL_RESULT_BUSY;
    }

    SP_SCOPED_NO_INTERRUPTS();

    if (SIIsChanBusy(sSIChannel)) {
        return POLL_RESULT_BUSY;
    }

    if (SIGetStatus(sSIChannel) & SI_ERROR_NO_RESPONSE) {
        u32 response[2];
        SIGetResponse(sSIChannel, response);
        return POLL_RESULT_RECONNECT;
    }

    u32 response[2];
    if (!SIGetResponse(sSIChannel, response)) {
        return POLL_RESULT_BUSY;
    }

    // errstat
    if (response[0] & 0x80000000) {
        return POLL_RESULT_RECONNECT;
    }
    // errlatch: ignored

    // input0
    if (response[0] & 0x20000000) {
        return POLL_RESULT_RECONNECT;
    }
    if (response[0] & 0x10000000) {
        return POLL_RESULT_RECONNECT;
    }

    // Validate checksum
    {
        const u8 checksum_data[7] = {
                (response[0] >> 24) & 0xff,
                (response[0] >> 16) & 0xff,
                (response[0] >> 8) & 0xff,
                (response[0] >> 0) & 0xff,

                (response[1] >> 24) & 0xff,
                (response[1] >> 16) & 0xff,
                (response[1] >> 8) & 0xff,
        };

        u32 checksum = 0;
        for (size_t i = 0; i < ARRAY_SIZE(checksum_data); ++i) {
            checksum ^= checksum_data[i];
        }

        if ((response[1] & 0xff) != checksum) {
            SP_LOG("Checksum failed");
            return POLL_RESULT_RECONNECT;
        }
    }

    pKeys->keys[0] = (response[1] >> 24) & 0xff;
    pKeys->keys[1] = (response[1] >> 16) & 0xff;
    pKeys->keys[2] = (response[1] >> 8) & 0xff;
    return POLL_RESULT_OK;
}

static bool sHasNextKeys = false;
static KeyCodeTriplet sLastKeys, sNextKeys;
static u32 sLastKeyPress = 0;
static bool sShiftState = false;

static TypingBuffer sTypingBuffer;
static bool sIsInit = false;

#define SIKeyboard_VerboseLog(...)

static const char sKeys[] = {
        [6] = '\0',   // HOME
        [7] = '\0',   // END
        [8] = '\0',   // PGUP
        [9] = '\0',   // PGDN
        [0xA] = '\0', // SCROLLLOCK

        [0x10] = 'a',
        'b',
        'c',
        'd',
        'e',
        'f',
        'g',
        'h',
        'i',
        'j',
        'k',
        'l',
        'm',
        'n',
        'o',
        'p',
        'q',
        'r',
        's',
        't',
        'u',
        'v',
        'w',
        'x',
        'y',
        'z',
        // 0x29
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '0',
        // 0x34
        '-',
        '`',
        '\0', // PRNTSCR
        '\'',
        '[',
        '=',
        '\'',
        ']', // HASH
        ',',
        '.',
        '/',
        '\\',          // INTERNATIONAL1
        [0x40] = '\0', // F1
        [0x4B] = '\0', // F12
        '\0',          // ESC
        '\0',          // INSERT
        '\0',          // DELETE
        ';',
        '\0', // BACKSPACE
        '\0', // TAB
        '\0', // 0x52: UNK
        '\0', // CAPSLOCK
        '\0', // LSHIFT
        '\0', // RSHIFT
        '\0', // LCTRL
        '\0', // RALT
        '\0', // LWIN
        ' ',
        '\0', // RWIN
        '\0', // MENU

        '\0', // LEFT
        '\0', // DOWN
        '\0', // UP
        'R',  // RIGHT

        '\0', // 0x60 UNK

        '\0', // ENTER
};
static const char sKeysShifted[] = {
        [6] = '\0',   // HOME
        [7] = '\0',   // END
        [8] = '\0',   // PGUP
        [9] = '\0',   // PGDN
        [0xA] = '\0', // SCROLLLOCK

        [0x10] = 'A',
        'B',
        'C',
        'D',
        'E',
        'F',
        'G',
        'H',
        'I',
        'J',
        'K',
        'L',
        'M',
        'N',
        'O',
        'P',
        'Q',
        'R',
        'S',
        'T',
        'U',
        'V',
        'W',
        'X',
        'Y',
        'Z',
        // 0x29
        '!',
        '@',
        '#',
        '$',
        '%',
        '^',
        '&',
        '*',
        '(',
        ')',
        // 0x34
        '_',
        '~',
        '\0', // PRNTSCR
        '\"',
        '{',
        '+',
        '"',
        '}', // HASH
        '<',
        '>',
        '?',
        '|',           // INTERNATIONAL1
        [0x40] = '\0', // F1
        [0x4B] = '\0', // F12
        '\0',          // ESC
        '\0',          // INSERT
        '\0',          // DELETE
        ':',
        '\0', // BACKSPACE
        '\0', // TAB
        '\0', // 0x52: UNK
        '\0', // CAPSLOCK
        '\0', // LSHIFT
        '\0', // RSHIFT
        '\0', // LCTRL
        '\0', // RALT
        '\0', // LWIN
        ' ',
        '\0', // RWIN
        '\0', // MENU

        '\0', // LEFT
        '\0', // DOWN
        '\0', // UP
        'R',  // RIGHT

        '\0', // 0x60 UNK

        '\0', // ENTER
};

bool SIKeyboard_KeycodeIsCharacter(char key) {
    return key < sizeof(sKeys) && sKeys[(size_t)key] != '\0';
}

char SIKeyboard_KeycodeToCharacter(char key, bool shift) {
    assert(key < sizeof(sKeys));
    if (key > sizeof(sKeys)) {
        return '\0';
    }
    return shift ? sKeysShifted[(size_t)key] : sKeys[(size_t)key];
}

static bool SIKeyboard_SetCurrentPacket(KeyCodeTriplet *keys) {
    const u32 tick = OSGetTick();
    // If a new keycode exists, replace the last
    if (sHasNextKeys) {
        sLastKeys = sNextKeys;
        sHasNextKeys = false;
    }
    // Duplicate keypress; accept repeats in 500ms intervals.
    if (KeyCodeTriplet_Equals(keys, &sLastKeys) &&
            OSTicksToMilliseconds(tick - sLastKeyPress) < 500) {
        sHasNextKeys = false;
        return false;
    }
    sLastKeyPress = tick;
    // Setup the next last packet
    sHasNextKeys = true;
    sNextKeys = *keys;
    return true;
}
static bool SIKeyboard_IsAlreadyHeld(char c) {
    for (size_t j = 0; j < 3; ++j) {
        if (sLastKeys.keys[j] == c) {
            return true;
        }
    }
    return false;
}

size_t SIKeyboard_Poll(char *pKeys, size_t max_keys) {
    SP_SCOPED_NO_INTERRUPTS();

    KeyCodeTriplet keys = (KeyCodeTriplet){.keys = {0, 0, 0}};
    PollResult pollResult = SIKeyboard_PollInternal(&keys);

    switch (pollResult) {
    case POLL_RESULT_BUSY:
        return 0;
    case POLL_RESULT_RECONNECT:
        SP_LOG("Terminating SIKeyboard connection");
        SIKeyboard_Disconnect();
        return 0;
    case POLL_RESULT_OK:
        break;
    }

    if (!SIKeyboard_SetCurrentPacket(&keys)) {
        return 0;
    }

    if (KeyCodeTriplet_IsEmpty(&keys)) {
        sShiftState = false;
        return 0;
    }

    size_t num = 0;

    // Move ctrls to front
    // TODO: Redo this entire file
    for (size_t i = 0; i < 3; ++i) {
        if (keys.keys[i] == SIKEY_LEFTCONTROL) {
            keys.keys[i] = 0;
            if (num < max_keys) {
                pKeys[num++] = SIKEY_LEFTCONTROL;
            } else {
                SP_LOG("SIKeyboard_Poll called with max_keys too small");
            }
        }
    }

    for (size_t i = 0; i < 3; ++i) {
        const char keycode = keys.keys[i];

        if (keycode == 0) {
            continue;
        }

        if (keycode == SIKEY_LEFTSHIFT || keycode == SIKEY_RIGHTSHIFT) {
            sShiftState = true;
            continue;
        }

        if (SIKeyboard_IsAlreadyHeld(keycode)) {
            continue;
        }

        if (sShiftState && num < max_keys) {
            sShiftState = false;
            pKeys[num++] = SIKEY_NEXT_IS_SHIFTED;
        }

        if (num < max_keys) {
            pKeys[num++] = keycode;
        } else {
            SP_LOG("SIKeyboard_Poll called with max_keys too small");
        }
    }

    return num;
}

s32 SIKeyboard_GetCurrentConnection(void) {
    return sSIChannel;
}

void SIKeyboard_Disconnect(void) {
    SP_SCOPED_NO_INTERRUPTS();

    SIDisablePolling(SI_CHAN_BIT(sSIChannel));
    sSIChannel = -1;
    sIsInit = false;
}

void SIKeyboard_PollingHandler(void) {
    SP_SCOPED_NO_INTERRUPTS();
    sIsInit = sSIChannel >= 0;

    char keybuf[3];
    const size_t num_keys = SIKeyboard_Poll(keybuf, sizeof(keybuf));
    if (num_keys < 1) {
        return;
    }

    SIKeyboard_VerboseLog("APPEND %c%c%c (new character size=%u, current buf size=%u)", keybuf[0],
            keybuf[1], keybuf[2], (u32)num_keys, (u32)sTypingBuffer.len);

    for (size_t i = 0; i < num_keys; ++i) {
        TypingBuffer_Append(&sTypingBuffer, keybuf[i]);
    }
}

size_t SIKeyboard_ConsumeBuffer(char *buf, size_t max_take) {
    SP_SCOPED_NO_INTERRUPTS();

    if (!sIsInit) {
        return 0;
    }

    const size_t taken = MIN(max_take, sTypingBuffer.len);

    if (taken > 0) {
        memcpy(buf, sTypingBuffer.buf, taken);

        TypingBuffer_DeleteFromFront(&sTypingBuffer, taken);
    }
    return taken;
}

void SIKeyboard_Connect(u32 chan) {
    SIRefreshSamplingRate();

    u32 response[2];
    SIGetResponse(chan, response);
    SISetCommand(chan, 0x540000);
    SIEnablePolling(SI_CHAN_BIT(chan));

    sSIChannel = chan;
    TypingBuffer_Init(&sTypingBuffer);
}

s32 SIKeyboard_Scan(void) {
    for (size_t chan = 0; chan < 4; ++chan) {
        u32 type = SIGetType(chan);

        SP_LOG("[%u] SI Type: %x", (u32)chan, type);
        if ((type & 0x08300000) == 0x08300000) {
            SP_LOG("Unsupported SIKeyboard variant");
            return -1;
        }
        if ((type & 0x08200000) == 0x08200000) {
            return chan;
        }
    }

    return -1;
}

void SIKeyboard_InitSimple(void) {
    const s32 kbd = SIKeyboard_Scan();

    if (kbd < 0) {
        SP_LOG("SIKeyboard: Not connected");
        return;
    }

    SIKeyboard_Connect(kbd);
    SP_LOG("SIKeyboard: Initialized");

    const bool registered = SIKeyboard_EnableBackgroundService();
    assert(registered);
}

bool SIKeyboard_EnableBackgroundService(void) {
    SP_LOG("Enabling background polling/interrupts");

    VIInit();

    // TODO: This might reduce PAD controller latency?
    SISetSamplingRate(1);
    return SIRegisterPollingHandler(SIKeyboard_PollingHandler);
}
