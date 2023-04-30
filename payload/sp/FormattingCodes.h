#pragma once

#include <sp/StringRange.h>

typedef enum {
    STATE_OBFUSCATED,
    STATE_BOLD,
    STATE_STRIKETHROUGH,
    STATE_UNDERLINE,
    STATE_ITALICS
} FontStates;

typedef struct {
    u16 mBC;
} Formatting;
extern const Formatting DefaultFormatting;

u8 Formatting_getColorCode(const Formatting f);
void Formatting_setColorCode(Formatting *f, u8 c);

bool Formatting_isState(const Formatting f, FontStates state);
void Formatting_setState(Formatting *f, FontStates state, bool v);

u32 hex_color_fg(u8 color_code);
u32 hex_color_bg(u8 color_code);

//! Apply a &n code:
//!
//! 0-9, a-f: colors (VGA 4-bit color palette 0)
//! l: STATE_BOLD
//! k: STATE_STRIKETHROUGH
//! n: STATE_UNDERLINE
//! o: STATE_ITALICS
//! r: clear state flags
//!
void Formatting_applyCode(Formatting *f, char action);

typedef struct {
    char character;
    Formatting code;
} FormattedChar;

typedef struct {
    Formatting font_state;
    StringRange mString;
} TextRange;

TextRange TextRange_create(const char *s, size_t len);
FormattedChar TextRange_next(TextRange *self);
