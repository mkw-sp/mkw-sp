#include "FormattingCodes.h"

// VGA 4-bit color palette 0
// Also the first line of 13h encoding
static const u32 hex_colors_fg[16] = {
        0x000000FF, // &0
        0x0000AAFF, // &1
        0x00AA00FF, // &2
        0x00AAAAFF, // &3
        0xAA0000FF, // &4
        0xAA00AAFF, // &5
        0xFFAA00FF, // &6
        0xAAAAAAFF, // &7
        0x555555FF, // &8
        0x5555FFFF, // &9
        0x55FF55FF, // &a
        0x55FFFFFF, // &b
        0xFF5555FF, // &c
        0xFF55FFFF, // &d
        0xFFFF55FF, // &e
        0xFFFFFFFF, // &f
};
u32 hex_color_fg(u8 color_code) {
    return hex_colors_fg[color_code & 0b1111];
}

static const u32 hex_colors_bg[16] = {
        0x000000FF, // &0
        0x00002AFF, // &1
        0x002A00FF, // &2
        0x002A2AFF, // &3
        0x2A0000FF, // &4
        0x2A002AFF, // &5
        0x2A2A00FF, // &6 original version, not bedrock
        0x2A2A2AFF, // &7
        0x151515FF, // &8
        0x15153FFF, // &9
        0x153F15FF, // &a
        0x153F3FFF, // &b
        0x3F1515FF, // &c
        0x3F153FFF, // &d
        0x3F3F15FF, // &e
        0x3F3F3FFF, // &f
};

u32 hex_color_bg(u8 color_code) {
    return hex_colors_bg[color_code & 0b1111];
}

const Formatting DefaultFormatting = (Formatting){
        .mBC = 0xf, // White + no effects
};

static inline u16 Formatting_flag(FontStates state, bool b) {
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
    f->mBC &= ~Formatting_flag(state, true);
    f->mBC |= Formatting_flag(state, v);
}

// Layout in memory:
// ByteCode: 0000000F FFFFCCCC
// - 0000000           : Unused
// -         FFFFF     : 5 format bits
// -              CCCC : 4 color bits
//
// Mask_Then_Or: AAAAAAAABBBBBBBB CCCCCCCCDDDDDDDD
// - AAAAAAAABBBBBBBB bits when enabled set the corresponding bytecode bit to 0
// - CCCCCCCCDDDDDDDD bits when enabled set the corresponding bytecode bit to 1
//
#define discard_color (0b0000000000001111 << 16)
#define discard_flags (0b1111111111110000 << 16)
#define color_set(x) discard_color | ((x)&0b1111)
#define flags_add(x) (1 << ((x) + 4))

#define mask_or_start '0'
#define mask_or_key(x) x - mask_or_start
static const u32 mask_or_table[] = {
        //
        [mask_or_key('0')] = color_set(0),
        [mask_or_key('1')] = color_set(1),
        [mask_or_key('2')] = color_set(2),
        [mask_or_key('3')] = color_set(3),
        [mask_or_key('4')] = color_set(4),
        [mask_or_key('5')] = color_set(5),
        [mask_or_key('6')] = color_set(6),
        [mask_or_key('7')] = color_set(7),
        [mask_or_key('8')] = color_set(8),
        [mask_or_key('9')] = color_set(9),

        [mask_or_key('a')] = color_set(0xa),
        [mask_or_key('b')] = color_set(0xb),
        [mask_or_key('c')] = color_set(0xc),
        [mask_or_key('d')] = color_set(0xd),
        [mask_or_key('e')] = color_set(0xe),
        [mask_or_key('f')] = color_set(0xf),

        [mask_or_key('k')] = flags_add(STATE_OBFUSCATED),
        [mask_or_key('l')] = flags_add(STATE_BOLD),
        [mask_or_key('m')] = flags_add(STATE_STRIKETHROUGH),
        [mask_or_key('n')] = flags_add(STATE_UNDERLINE),
        [mask_or_key('o')] = flags_add(STATE_ITALICS),

        [mask_or_key('r')] = discard_flags,
};
#undef discard_color
#undef discard_flags
#undef flag_set
#undef color_set

void Formatting_applyCode(Formatting *f, char action) {
    const size_t idx = (size_t)action;
    if (idx < mask_or_start || mask_or_key(idx) >= ARRAY_SIZE(mask_or_table)) {
        return;
    }
    const u32 action_v = mask_or_table[mask_or_key(idx)];
    const u16 mask_v = ~(action_v >> 16);
    const u16 or_v = action_v & 0xFFFF;

    f->mBC = (f->mBC & mask_v) | or_v;
}
#undef mask_or_start
#undef mask_or_key

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
                    .code = {0},
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
                .code = self->font_state,
        };
    }
}
