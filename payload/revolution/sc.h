#pragma once

enum {
    SC_LANG_JAPANESE = 0x0,
    SC_LANG_ENGLISH = 0x1,
    SC_LANG_GERMAN = 0x2,
    SC_LANG_FRENCH = 0x3,
    SC_LANG_SPANISH = 0x4,
    SC_LANG_ITALIAN = 0x5,
    SC_LANG_DUTCH = 0x6,
    SC_LANG_SIMP_CHINESE = 0x7,
    SC_LANG_TRAD_CHINESE = 0x8,
    SC_LANG_KOREAN = 0x9,
};

u8 SCGetLanguage(void);

u32 SCGetCounterBias(void);
