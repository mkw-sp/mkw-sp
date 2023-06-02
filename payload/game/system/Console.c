#include "Console.h"
#include "TextWriter.h"

#include <revolution.h>
#include <sp/ScopeLock.h>
#include <sp/keyboard/Keyboard.h>
#include <stdio.h>
#include <string.h>

static bool sInit = false;
static TextWriter sConsole;
static bool sLineVisible = false;
static float sConsoleAlpha = 1.0f;
static float sLineAlpha = 0.0f;
static u32 sFramesSinceLastOpen = 1000;
static char sLastLine[64];

enum {
    LINES = 24,
    LINE_SIZE = 256,
};
static char sHistory[LINES][LINE_SIZE];
static size_t sHistoryCursor = 0;
static void AppendToHistory(const char *s) {
    if (sHistoryCursor >= LINES) {
        memmove(sHistory, sHistory + 1, LINE_SIZE * (LINES - 1));
        --sHistoryCursor;
    }
    assert(sHistoryCursor < LINES);
    snprintf(sHistory[sHistoryCursor++], LINE_SIZE - 1, "&r&f%s", s);
}

static void Console_create() {
    Rect box = {
            .top = 270.0f,
            .bottom = 800.0f,
            .left = 10.0f,
            .right = 600.0f,
    };
    TextWriter_configure(&sConsole, box, NULL);
}

static void Console_stateDefault(int frames_since_last_console_message) {
    if (frames_since_last_console_message < 120) {
        sConsoleAlpha = 1.0f;
    } else if (frames_since_last_console_message < 240) {
        sConsoleAlpha = ((float)(240 - frames_since_last_console_message)) / 120.0f;
    } else {
        sConsoleAlpha = 0.0f;
    }
    if (frames_since_last_console_message < 15) {
        sLineAlpha = ((float)(15 - frames_since_last_console_message)) / 15.0f;
        sLineVisible = true;
    } else {
        sLineAlpha = 0.0f;
        sLineVisible = false;
    }
}
static void Console_stateTyping() {
    sLineVisible = true;
    sConsoleAlpha = 1.0f;
}

static void Console_drawImpl() {
    const SP_Line line = SP_GetCurrentLine();
    if (line.len != 0) {
        memcpy(sLastLine, line.buf, MIN(line.len, sizeof(sLastLine) - 1));
        sLastLine[MIN(line.len, sizeof(sLastLine) - 1)] = '\0';
    }

    const float font_size = 7.0f;

    if (sConsoleAlpha == 0.0f && !sLineVisible) {
        return;
    }

    TextWriter_reset(&sConsole);
    TextWriter_beginDraw(&sConsole);
    if (sConsoleAlpha != 0.0f) {
        const u32 alpha_quantized = (u8)(sConsoleAlpha * 255.0f);
        for (size_t i = 0; i < sHistoryCursor; ++i) {
            const char *line = &sHistory[i][0];
            TextWriter_draw(&sConsole, line, strlen(line), font_size, alpha_quantized);
        }
    }
    if (sLineVisible) {
        const u32 alpha_quantized = (u8)(sLineAlpha * 255.0f);
        TextWriter_draw(&sConsole, sLastLine, strlen(sLastLine), font_size, alpha_quantized);
    }
    TextWriter_endDraw(&sConsole);
}

static OSMutex sConsoleMutex;

void Console_init(void) {
    if (sInit) {
        return;
    }
    Console_create();
    OSInitMutex(&sConsoleMutex);
    sInit = true;
}
void Console_draw(void) {
    if (!sInit || !SP_IsConsoleInputInit()) {
        return;
    }
    SP_SCOPED_MUTEX_LOCK(sConsoleMutex);
    Console_drawImpl();
}
void Console_calc(void) {
    if (!sInit || !SP_IsConsoleInputInit()) {
        return;
    }
    SP_SCOPED_MUTEX_LOCK(sConsoleMutex);
    if (SP_IsTyping()) {
        sFramesSinceLastOpen = 0;
        Console_stateTyping();
        return;
    }
    Console_stateDefault(sFramesSinceLastOpen++);
}
void Console_addLine(const char *s, size_t /* len */) {
    // To support being called by an interrupt handler, we can't use a mutex. If a call
    // was interrupted, the global state could be accesesd in an invalid state.
    SP_SCOPED_NO_INTERRUPTS();

#if 0
    OSThread *thread = OSGetCurrentThread();
    if (thread == NULL) {
        if (thread == NULL) {
            va_list l;
            vprintf("Called with NULL current thread (hack to prevent optimization to "
                    "puts, which dolphin doesn't detect: garbage int %i)\n",
                    l);
        }
    }
#endif
    AppendToHistory(s);
}

static void Console_vprintfThreadUnsafe(const char *prefix, const char *s, va_list args) {
    // 1024 bytes of stack may be too large for some threads, so we store it in BSS.
    static char formatted[512];
    vsnprintf(formatted, sizeof(formatted), s, args);

    static char buf[512];
    const size_t len = snprintf(buf, sizeof(buf), "%s%s", prefix, formatted);

    Console_addLine(buf, len);
}

void Console_vprintf(const char *prefix, const char *s, va_list args) {
    // To support being called by an interrupt handler, we can't use a mutex. If a call
    // was interrupted, the global state could be accesesd in an invalid state.
    SP_SCOPED_NO_INTERRUPTS();

    Console_vprintfThreadUnsafe(prefix, s, args);
}
