#include "FormattingCodes.h"

// VGA 4-bit color palette 0
// Also the first line of 13h encoding
static const u32 hex_colors_fg[16] = {
    0x000000FF,  // &0
    0x0000AAFF,  // &1
    0x00AA00FF,  // &2
    0x00AAAAFF,  // &3
    0xAA0000FF,  // &4
    0xAA00AAFF,  // &5
    0xFFAA00FF,  // &6
    0xAAAAAAFF,  // &7
    0x555555FF,  // &8
    0x5555FFFF,  // &9
    0x55FF55FF,  // &a
    0x55FFFFFF,  // &b
    0xFF5555FF,  // &c
    0xFF55FFFF,  // &d
    0xFFFF55FF,  // &e
    0xFFFFFFFF,  // &f
};
u32 hex_color_fg(u8 color_code) {
    return hex_colors_fg[color_code & 0b1111];
}

static const u32 hex_colors_bg[16] = {
    0x000000FF,  // &0
    0x00002AFF,  // &1
    0x002A00FF,  // &2
    0x002A2AFF,  // &3
    0x2A0000FF,  // &4
    0x2A002AFF,  // &5
    0x2A2A00FF,  // &6 original version, not bedrock
    0x2A2A2AFF,  // &7
    0x151515FF,  // &8
    0x15153FFF,  // &9
    0x153F15FF,  // &a
    0x153F3FFF,  // &b
    0x3F1515FF,  // &c
    0x3F153FFF,  // &d
    0x3F3F15FF,  // &e
    0x3F3F3FFF,  // &f
};

u32 hex_color_bg(u8 color_code) {
    return hex_colors_bg[color_code & 0b1111];
}

const Formatting DefaultFormatting = (Formatting){
    .mBC = 0xf,  // White + no effects
};

static inline u8 Formatting_flag(FontStates state, bool b) {
    return b << (4 + state);
}

u8 Formatting_getColorCode(const Formatting f) {
    return f.mBC & 0b1111;
}
void Formatting_setColorCode(Formatting *f, u8 c) {
    f->mBC = (f->mBC & ~0b1111) | (c & 0b1111);
}
bool Formatting_isState(const Formatting f, FontStates state) {
    return f.mBC & Formatting_flag(state, true);
}
void Formatting_setState(Formatting *f, FontStates state, bool v) {
    u8 tmp = f->mBC & ~Formatting_flag(state, true);
    f->mBC = tmp | Formatting_flag(state, v);
}

// Layout in memory:
// ByteCode: FFFFCCCC
// - FFFF: 4 format bits
// - CCCC: 4 color bits
//
// Mask_Then_Or: AAAABBBB CCCCDDDD
// - AAAABBBB bits when enabled set the corresponding bytecode bit to 0
// - CCCCDDDD bits when enabled set the corresponding bytecode bit to 1
//
#define discard_color (0b00001111 << 8)
#define discard_flags (0b11110000 << 8)
#define flag_set(x) (1 << ((x) + 4))
#define color_set(x) ((x)&0b1111)
static const u16 mask_or_table[0xFF] = {
    //
    ['0'] = discard_color | color_set(0),
    ['1'] = discard_color | color_set(1),
    ['2'] = discard_color | color_set(2),
    ['3'] = discard_color | color_set(3),
    ['4'] = discard_color | color_set(4),
    ['5'] = discard_color | color_set(5),
    ['6'] = discard_color | color_set(6),
    ['7'] = discard_color | color_set(7),
    ['8'] = discard_color | color_set(8),
    ['9'] = discard_color | color_set(9),

    ['a'] = discard_color | color_set(0xa),
    ['b'] = discard_color | color_set(0xb),
    ['c'] = discard_color | color_set(0xc),
    ['d'] = discard_color | color_set(0xd),
    ['e'] = discard_color | color_set(0xe),
    ['f'] = discard_color | color_set(0xf),

    ['l'] = flag_set(STATE_BOLD),
    ['m'] = flag_set(STATE_STRIKETHROUGH),
    ['n'] = flag_set(STATE_UNDERLINE),
    ['o'] = flag_set(STATE_ITALICS),

    // Discard all flags
    ['r'] = (0b11110000 << 8),
};
#undef discard_color
#undef flag_set
#undef color_set

void Formatting_applyCode(Formatting *f, char action) {
    u16 action_v = mask_or_table[(size_t)action];
    u8 mask_v = ~(action_v >> 8);
    u8 or_v = action_v & 0xFF;

    f->mBC = (f->mBC & mask_v) | or_v;
}

TextRange TextRange_create(const char *s, size_t len) {
    return (TextRange){
        .font_state = DefaultFormatting,
        .mString = StringRange_create(s, len),
    };
}

FormattedChar TextRange_next(TextRange *self) {
    bool expect_code = false;

    while (true) {
        const char c = StringRange_next(&self->mString);

        if (c == '\0') {
            return (FormattedChar){
                .character = '\0',
                .code = 0,
            };
        }

        if (c == '&') {
            expect_code = true;
            continue;
        }

        if (expect_code) {
            expect_code = false;

            Formatting_applyCode(&self->font_state, c);
            continue;
        }

        return (FormattedChar){
            .character = c,
            .code = self->font_state.mBC,
        };
    }
}
