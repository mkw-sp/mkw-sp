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

static s32 sKeyboardChannel = -1;

static char sLastKeys[3] = { 0, 0, 0 };
static u32 sLastKeyPress = 0;
static bool sShiftState = false;

static TypingBuffer sTypingBuffer;
static bool sIsInit = false;

#define SIKeyboard_VerboseLog(...)

static bool SIKeyboard_TranslateKey(char *pKey) {
    assert(pKey != NULL);
    char key = *pKey;

    char c = '?';
    if (key >= 0x10 && key <= 0x29) {
        c = key - 0x10 + 'a';
    } else if (key == 0x54) {
        SIKeyboard_VerboseLog("APPLY SHIFT");
        sShiftState = true;
        return false;
    } else if (key == 0x3e) {
        c = '/';
    } else if (key == 0x34) {
        c = sShiftState ? '_' : '-';
    } else if (key == 0x61) {
        c = 0xff;
    } else if (key == 0x50) {
        c = 0xfe;
    } else if (key == 0) {
        // Sustain shift press
        return false;
    } else {
        if (sShiftState) {
            SIKeyboard_VerboseLog("DROP SHIFT %x", (u32)key);
        }
        sShiftState = false;
        return false;
    }

    *pKey = c;
    if (sShiftState) {
        SIKeyboard_VerboseLog("DROP SHIFT %c", c);
    }
    sShiftState = false;
    return true;
}
static u32 sLast[2];
// shouldTerminate: If called from a polling handler
static size_t SIKeyboard_PollInternal(char *pKeys, bool *shouldTerminate, s32 *seq) {
    if (shouldTerminate != NULL)
        *shouldTerminate = false;
    if (seq != NULL)
        *seq = -1;
    if (sKeyboardChannel < 0) {
        return 0;
    }

    SP_SCOPED_NO_INTERRUPTS();

    if (SIIsChanBusy(sKeyboardChannel)) {
        return 0;
    }

    if (SIGetStatus(sKeyboardChannel) & SI_ERROR_NO_RESPONSE) {
        u32 response[2];
        SIGetResponse(sKeyboardChannel, response);
        if (shouldTerminate != NULL)
            *shouldTerminate = true;
        return 0;
    }

    u32 response[2];
    if (!SIGetResponse(sKeyboardChannel, response)) {
        return 0;
    }
    if (sLast[0] != response[0] || sLast[1] != response[1]) {
        SIKeyboard_VerboseLog("RESPONSE %x %x", response[0], response[1]);
    }

    sLast[0] = response[0];
    sLast[1] = response[1];

    // errstat
    if (response[0] & 0x80000000) {
        if (shouldTerminate != NULL)
            *shouldTerminate = true;
        return 0;
    }
    // errlatch: ignored

    // input0
    if (response[0] & 0x20000000) {
        if (shouldTerminate != NULL)
            *shouldTerminate = true;
        return 0;
    }
    if (response[0] & 0x10000000) {
        if (shouldTerminate != NULL)
            *shouldTerminate = true;
        return 0;
    }
    if (seq != NULL)
        *seq = (response[0] >> 24) & 15;

    // Validate checksum
    {
        u8 checksum_data[7];

        checksum_data[0] = (response[0] >> 24) & 0xff;
        checksum_data[1] = (response[0] >> 16) & 0xff;
        checksum_data[2] = (response[0] >> 8) & 0xff;
        checksum_data[3] = (response[0] >> 0) & 0xff;

        checksum_data[4] = (response[1] >> 24) & 0xff;
        checksum_data[5] = (response[1] >> 16) & 0xff;
        checksum_data[6] = (response[1] >> 8) & 0xff;

        u32 checksum = 0;
        for (size_t i = 0; i < ARRAY_SIZE(checksum_data); ++i) {
            checksum ^= checksum_data[i];
        }

        if ((response[1] & 0xff) != checksum) {
            if (shouldTerminate != NULL)
                *shouldTerminate = true;
            SP_LOG("Checksum failed");
            return false;
        }
    }

    pKeys[0] = (response[1] >> 24) & 0xff;
    pKeys[1] = (response[1] >> 16) & 0xff;
    pKeys[2] = (response[1] >> 8) & 0xff;
    return 3;
}

size_t SIKeyboard_Poll(char *pKeys, size_t max_keys) {
    SP_SCOPED_NO_INTERRUPTS();

    bool shouldTerminate = false;
    char keys[3] = { 0, 0, 0 };
    SIKeyboard_PollInternal(keys, &shouldTerminate, NULL);

    if (shouldTerminate) {
        SP_LOG("Terminating keyboard connection");
        SIKeyboard_Terminate();
        return 0;
    }

    // Unfortunate workaround
    // TODO: First press longer wait than subsequent when holding down key
    const u32 tick = OSGetTick();
    if (!memcmp(&keys, sLastKeys, sizeof(keys)) &&
            OSTicksToMilliseconds(tick - sLastKeyPress) < 500) {
        return 0;
    }

    if (keys[0] == 0 && keys[1] == 0 && keys[2] == 0) {
        if (sShiftState) {
            SIKeyboard_VerboseLog("ALL ZERO: Drop shift");
        }
        sShiftState = false;
        return 0;
    }

    size_t num = 0;
    for (size_t i = 0; i < 3; ++i) {
        char c = keys[i];

        if (c != 0x54 /* SHIFT */) {
            bool already_pressed = false;
            for (size_t j = 0; j < 3; ++j) {
                if (sLastKeys[j] == c) {
                    already_pressed = true;
                }
            }
            if (already_pressed) {
                continue;
            }
        }

        if (!SIKeyboard_TranslateKey(&c)) {
            SIKeyboard_VerboseLog("Failed to translate");
            continue;
        }

        if (max_keys >= i) {
            *pKeys = c;
            SIKeyboard_VerboseLog("ADDED");
            ++num;
            continue;
        }
        SP_LOG("Invalid max keys");
        break;
    }

    for (int i = 0; i < 3; ++i) {
        sLastKeys[i] = keys[i];
    }
    sLastKeyPress = tick;
    return num;
}

s32 SIKeyboard_GetActiveChannel(void) {
    return sKeyboardChannel;
}

void SIKeyboard_Terminate(void) {
    SIDisablePolling(SI_CHAN_BIT(sKeyboardChannel));
    sKeyboardChannel = -1;
}

void SIKeyboard_PollingHandler(void) {
    for (int i = 0; i < 1; ++i) {
        sIsInit = true;

        // SP_LOG("POLLING HANDLER");
        char c[3];
        if (SIKeyboard_Poll(c, sizeof(c)) < 1) {
            continue;
        }

        SP_SCOPED_NO_INTERRUPTS();
        SIKeyboard_VerboseLog("APPEND %c (len=%u)", c, sTypingBuffer.len);
        TypingBuffer_Append(&sTypingBuffer, c[i]);
        // SP_LOG("BUF: %s", &sTypingBuffer.buf);
    }
}

size_t SIKeyboard_ConsumeBuffer(char *buf, size_t max_take) {
    SP_SCOPED_NO_INTERRUPTS();

    if (!sIsInit) {
        return 0;
    }

    // SP_LOG("Buffered: %s %u", sTypingBuffer.buf, (u32)sTypingBuffer.len);
    const size_t taken = MIN(max_take, sTypingBuffer.len);
    // SP_LOG("Taken %u", (u32)taken);

    if (taken > 0) {
        memcpy(buf, sTypingBuffer.buf, taken);

        const size_t remaining = sTypingBuffer.len - taken;
        if (remaining > 0) {
            memmove(sTypingBuffer.buf, sTypingBuffer.buf + taken, remaining);
        }
        sTypingBuffer.len = remaining;

        if (sTypingBuffer.len < 64) {
            sTypingBuffer.buf[sTypingBuffer.len] = '\0';
        }
    }
    return taken;
}

void SIKeyboard_Init(u32 chan) {
    SIRefreshSamplingRate();

    u32 response[2];
    for (u32 chan = 0; chan < 4; ++chan) {
        SIGetResponse(chan, response);
        SISetCommand(chan, 0x00400300);
        SIEnablePolling(SI_CHAN_BIT(chan));
    }

    OSSleepMilliseconds(10);

    SIGetResponse(chan, response);
    SISetCommand(chan, 0x540000);
    SIEnablePolling(SI_CHAN_BIT(chan));

    sKeyboardChannel = chan;
    TypingBuffer_Init(&sTypingBuffer);
}

s32 SIKeyboard_Scan(void) {
    for (int chan = 0; chan < 4; ++chan) {
        u32 type = SIGetType(chan);

        SP_LOG("TYPE: %x", type);
        // TODO: Actual driver supports wireless variant, handles extra info
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

    SIKeyboard_Init(kbd);
    SP_LOG("SIKeyboard: Initialized");

    const bool registered = SIKeyboard_EnableBackgroundService();
    assert(registered);
}

bool SIKeyboard_EnableBackgroundService(void) {
    SP_LOG("Enabling background polling/interrupts");
    VIInit();
    SISetSamplingRate(1);
    return SIRegisterPollingHandler(SIKeyboard_PollingHandler);
}