#include <game/host_system/SystemManager.h>
#include <game/ui/Font.h>
#include <game/ui/FontManager.h>
#include <nw4r/lyt/lyt_layout.h>
#include <revolution.h>
#include <sp/FormattingCodes.h>
#include <sp/ScopeLock.h>
#include <sp/keyboard/Keyboard.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    float proj_mtx[4][4];
    float width;
    float height;
    float pos_mtx[3][4];
} ScreenDrawData;

static void ScreenDrawData_create(ScreenDrawData *out) {
    C_MTXOrtho(out->proj_mtx, 0.0, 456.0f, // Top/Bottom
            0.0, 608.0f,                   // Left/Right
            0.0f, -1.0f                    // Clip plane
    );
    out->width = 608;
    out->height = 456;
    GXSetViewport(0.0, 0.0,          // left/top
            out->width, out->height, // width/height
            0.0, 1.0                 // clip plane
    );

    PSMTXIdentity(out->pos_mtx);
}
static void ScreenDrawData_apply(ScreenDrawData *self, u32 matrix_slot) {
    GXSetScissor(0, 0,                          // Left/Top
            (u16)self->width, (u16)self->height // width/height
    );
    GXSetProjection(self->proj_mtx, GX_ORTHOGRAPHIC);
    GXLoadPosMtxImm(self->pos_mtx, matrix_slot);
    GXSetCurrentMtx(matrix_slot);
}

typedef struct {
    float cursor_x;
    float cursor_y;
    float max_height; // For the current line
    Rect base_region;
} WriterTextBox;

static void WriterTextBox_reset(WriterTextBox *self) {
    self->cursor_x = self->base_region.left;
    self->cursor_y = self->base_region.top;
    self->max_height = 0.0f;
}
static WriterTextBox WriterTextBox_create(Rect base) {
    WriterTextBox result;
    result.base_region = base;
    WriterTextBox_reset(&result);
    return result;
}
static void WriterTextBox_newLine(WriterTextBox *self) {
    self->cursor_y += self->max_height;
    self->cursor_x = self->base_region.left;
    self->max_height = 0.0f;
}

static Rect WriterTextBox_advanceCursor(WriterTextBox *self, float width, float height,
        bool isSpace) {
    // Calc line wrap
    float indentThreshhold = width;

    // Strongly favor word breaks
    if (isSpace) {
        indentThreshhold = 10.0f * width;
    }

    if (self->base_region.right - (width + self->cursor_x) < indentThreshhold) {
        WriterTextBox_newLine(self);
        // Indent
        self->cursor_x += width;
    }

    Rect result = (Rect){
            .top = self->cursor_y,
            .bottom = self->cursor_y + height,
            .left = self->cursor_x,
            .right = self->cursor_x + width,
    };

    self->cursor_x += width;

    if (height > self->max_height) {
        self->max_height = height;
    }

    return result;
}

typedef struct {
    WriterTextBox mBox;
    ScreenDrawData mScreen;
    void *mLastTexObj;
} TextWriter;
static void TextWriter_configure(TextWriter *self, Rect box, void * /* res */) {
    self->mBox = WriterTextBox_create(box);
    self->mLastTexObj = NULL;
    ScreenDrawData_create(&self->mScreen);
}
static void TextWriter_reset(TextWriter *self) {
    WriterTextBox_reset(&self->mBox);
    self->mLastTexObj = NULL;
}
static void TextWriter_beginDraw(TextWriter *self) {
    lyt_TextBox textBox;
    GlyphRenderer renderer;
    renderer.textBox = &textBox;
    GlyphRenderer_setMaterial(&renderer);
    GXSetNumTevStages(1);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

    ScreenDrawData_apply(&self->mScreen, /* matrix_slot= */ 0);
    GXSetColorUpdate(1);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetCullMode(GX_CULL_FRONT);
    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
}
static void TextWriter_endDraw(TextWriter * /* writer */) {}
enum {
    VERT_LEFT_X,
    VERT_RIGHT_X,
    VERT_TOP_Y,
    VERT_BOTTOM_Y,
};
static void DrawColoredQuad(GXColor fg_color, GXColor bg_color, const s16 *pos, const u16 *uv) {
    GXSetTevColor(GX_TEVREG0, fg_color);
    GXSetTevColor(GX_TEVREG1, bg_color);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition2s16(pos[VERT_RIGHT_X], pos[VERT_BOTTOM_Y]);
        GXTexCoord2u16(uv[VERT_RIGHT_X], uv[VERT_BOTTOM_Y]);

        GXPosition2s16(pos[VERT_RIGHT_X], pos[VERT_TOP_Y]);
        GXTexCoord2u16(uv[VERT_RIGHT_X], uv[VERT_TOP_Y]);

        GXPosition2s16(pos[VERT_LEFT_X], pos[VERT_TOP_Y]);
        GXTexCoord2u16(uv[VERT_LEFT_X], uv[VERT_TOP_Y]);

        GXPosition2s16(pos[VERT_LEFT_X], pos[VERT_BOTTOM_Y]);
        GXTexCoord2u16(uv[VERT_LEFT_X], uv[VERT_BOTTOM_Y]);
    }
    GXEnd();
}

static void TextWriter_drawQuad(TextWriter *self, GXColor fg_color, GXColor bg_color, GXTexObj *obj,
        RectU16 uv_encoded, Rect position) {
    assert(obj);
    if (self->mLastTexObj != obj) {
        GXLoadTexObj(obj, GX_TEXMAP0);
        self->mLastTexObj = obj;
    }

    {
        const s16 pos[4] = {
                [VERT_LEFT_X] = encodeSFixed6(position.left),
                [VERT_RIGHT_X] = encodeSFixed6(position.right),
                [VERT_TOP_Y] = encodeSFixed6(position.top),
                [VERT_BOTTOM_Y] = encodeSFixed6(position.bottom),
        };
        const u16 uv[4] = {
                [VERT_LEFT_X] = uv_encoded.left,
                [VERT_RIGHT_X] = uv_encoded.right,
                [VERT_TOP_Y] = uv_encoded.top,
                [VERT_BOTTOM_Y] = uv_encoded.bottom,
        };
        DrawColoredQuad(bg_color, fg_color, pos, uv);
    }
}

static void TextWriter_drawCharByColors(TextWriter *self, char c, GXColor fg_color,
        GXColor bg_color, float size) {
    assert(sDebugFont != NULL);
    const FontInformation *info = sDebugFont->base.fontInformation;
    assert(info != NULL);
    const float fontSize[2] = {(float)info->mWidth, (float)info->mHeight};

    RKFontGlyphQuad glyph;
    Font_calcQuad(sDebugFont, &glyph, c);

    float width = size * fontSize[0] * decodeFixed15(glyph.uv.right - glyph.uv.left);
    float height = size * fontSize[1] * decodeFixed15(glyph.uv.bottom - glyph.uv.top);

    // 16:9 adjustment
    if (s_systemManager->aspectRatio) {
        width *= 0.75f;
    }

    const Rect screen_pos = WriterTextBox_advanceCursor(&self->mBox, width, height, c == '-');

    TextWriter_drawQuad(self,   //
            fg_color, bg_color, // Color
            glyph.tex_obj,      // Texture
            glyph.uv,           // UV
            screen_pos          // Position
    );
}

static GXColor HexColorToGXColor(u32 hexColor) {
    return (GXColor){
            .r = hexColor >> 24,
            .g = hexColor >> 16,
            .b = hexColor >> 8,
            .a = hexColor >> 0,
    };
}

static void TextWriter_drawCharByFormatCode(TextWriter *self, char c, Formatting format, float size,
        u8 alpha_override) {
    GXColor fg_color = HexColorToGXColor(hex_color_fg(Formatting_getColorCode(format)));
    GXColor bg_color = HexColorToGXColor(hex_color_bg(Formatting_getColorCode(format)));

    if (Formatting_isState(format, STATE_BOLD)) {
        size *= 1.2;
    }

    if (alpha_override != 0) {
        fg_color.a = alpha_override;
        bg_color.a = alpha_override;
    }

    if (Formatting_isState(format, STATE_OBFUSCATED)) {
        const char *dict = "1234567890abcdefghijklmnopqrstuvwxyz~!@#$%^&*()-=_+{}[]";
        size_t dict_len = strlen(dict);
        c = dict[OSGetTick() % dict_len];
    }

    // TODO: italic, etc
    TextWriter_drawCharByColors(self, c, fg_color, bg_color, size);
}

static void TextWriter_newLine(TextWriter *self, float size) {
    // Invisible char to compute line height
    GXColor transparent = {0, 0, 0, 0};
    TextWriter_drawCharByColors(self, 'F', transparent, transparent, size);
    WriterTextBox_newLine(&self->mBox);
}
// alpha_override = 0 -> no override
static void TextWriter_draw(TextWriter *self, const char *str, u32 len, float size,
        u8 alpha_override) {
    TextRange range = TextRange_create(str, len);

    while (true) {
        FormattedChar fc = TextRange_next(&range);

        switch (fc.character) {
        case '\0':
            return;
        case '\n':
            TextWriter_newLine(self, size);
            break;
        case ' ':;
            // Invisible char
            GXColor transparent = {0, 0, 0, 0};
            TextWriter_drawCharByColors(self, '-', transparent, transparent, size);
            break;
        default:
            TextWriter_drawCharByFormatCode(self, fc.character, fc.code, size, alpha_override);
            break;
        }
    }
}

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
            .top = 10.0f,
            .bottom = 800.0f,
            .left = 10.0f,
            .right = 1200.0f,
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
    /*const SP_Line line = SP_GetCurrentLine();
    if (line.len != 0) {
        memcpy(sLastLine, line.buf, MIN(line.len, sizeof(sLastLine) - 1));
        sLastLine[MIN(line.len, sizeof(sLastLine) - 1)] = '\0';
    }*/

    const float font_size = 14.0f;

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
    if (!sInit/* || !SP_IsConsoleInputInit()*/) {
        return;
    }
    SP_SCOPED_MUTEX_LOCK(sConsoleMutex);
    Console_drawImpl();
}
void Console_calc(void) {
    if (!sInit/* || !SP_IsConsoleInputInit()*/) {
        return;
    }
    SP_SCOPED_MUTEX_LOCK(sConsoleMutex);
    /*if (SP_IsTyping()) {*/
        sFramesSinceLastOpen = 0;
        Console_stateTyping();
        return;
    /*}
    Console_stateDefault(sFramesSinceLastOpen++);*/
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
