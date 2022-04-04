#pragma once

#include <Common.h>

// Based on OGWS
// https://github.com/doldecomp/ogws/blob/master/include/nw4r/ut/ut_ResFontBase.h

typedef struct {
    char BYTE_0x0;
    char BYTE_0x1;
    char BYTE_0x2;
} CharWidths;

typedef struct {
    char UNK_0x0;
    s8 mLineFeed;  // at 0x1

    u16 mAlternateChar;

    CharWidths mDefaultCharWidths;  // at 0x4
    u8 mEncoding;                   // at 0x7
    void *mTextureGlyph;
    void *mWidthList;
    void *mCodeMapList;
    u8 mHeight;  // at 0x14
    u8 mWidth;   // at 0x15

    u8 mAscent;  // at 0x16
} FontInformation;

typedef struct {
    void *vt;
    u32 readerFunc[3];  // ptmf
    void *buffer;
    FontInformation *fontInformation;
    u16 lastCharCode;
    u16 lastGlyph;
} ut_ResFont;

void ResFont_SetResource(ut_ResFont *self, const void *res);
