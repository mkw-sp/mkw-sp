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

static u32 sLastKey = -1;
static u32 sLastKeyPress = 0;
static bool sShiftState = false;

static TypingBuffer sTypingBuffer;
static bool sIsInit = false;

static bool SIKeyboard_TranslateKey(char *pKey) {
    assert(pKey != NULL);
    char key = *pKey;

    char c = '?';
    if (key >= 0x10 && key <= 0x29) {
        c = key - 0x10 + 'a';
    } else if (key == 0x54) {
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
    } else {
        sShiftState = false;
        return false;
    }

    *pKey = c;
    sShiftState = false;
    return true;
}

// shouldTerminate: If called from a polling handler
static u32 SIKeyboard_PollInternal(bool *shouldTerminate, s32 *seq) {
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
    // SP_LOG("RESPONSE %x %x", response[0], response[1]);

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
            return false;
        }
    }

    return response[1] >> 24;
}

size_t SIKeyboard_Poll(char *pKeys, size_t max_keys) {
    SP_SCOPED_NO_INTERRUPTS();

    bool shouldTerminate = false;
    const u32 keys = SIKeyboard_PollInternal(&shouldTerminate, NULL);
    if (shouldTerminate) {
        SP_LOG("Terminating keyboard connection");
        SIKeyboard_Terminate();
        return 0;
    }

    if (keys == 0) {
        return 0;
    }

    // Unfortunate workaround
    // TODO: First press longer wait than subsequent when holding down key
    const u32 tick = OSGetTick();
    if (keys == sLastKey && OSTicksToMilliseconds(tick - sLastKeyPress) < 500) {
        // SP_LOG("Duplicate key");
        return 0;
    }
    sLastKey = keys;
    sLastKeyPress = tick;

    // SP_LOG("Keys=%x", keys);
    u32 key = (keys & 0xff);

    char c = key & 0xff;
    if (!SIKeyboard_TranslateKey(&c)) {
        SP_LOG("Failed to translate");
        return 0;
    }

    if (max_keys >= 1) {
        *pKeys = c;
        SP_LOG("ADDED");
        return 1;
    }
    SP_LOG("Invalid max keys");
    return 0;
}

s32 SIKeyboard_GetActiveChannel(void) {
    return sKeyboardChannel;
}

void SIKeyboard_Terminate(void) {
    SIDisablePolling(SI_CHAN_BIT(sKeyboardChannel));
    sKeyboardChannel = -1;
}

void SIKeyboard_PollingHandler(void) {
    SP_SCOPED_NO_INTERRUPTS();

    sIsInit = true;

    // SP_LOG("POLLING HANDLER");
    char c;
    if (SIKeyboard_Poll(&c, 1) < 1) {
        return;
    }

    SP_LOG("APPEND %c (len=%u)", c, sTypingBuffer.len);
    TypingBuffer_Append(&sTypingBuffer, c);
    // SP_LOG("BUF: %s", &sTypingBuffer.buf);
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
    return SIRegisterPollingHandler(SIKeyboard_PollingHandler);
}