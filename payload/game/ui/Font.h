#pragma once

#include "../system/Mii.h"

typedef struct {
    u8 _00[0x24 - 0x00];
} Font;
static_assert(sizeof(Font) == 0x24);

Font *Font_ct(Font *this);

void Font_load(Font *this, const char *file);

typedef struct {
    s32 intVals[9];
    u32 messageIds[9];
    Mii *miis[9];
    u8 _6c[0x9c - 0x6c];
    wchar_t *strings[9];
    u8 _c0[0xc4 - 0xc0];
} MessageInfo;
static_assert(sizeof(MessageInfo) == 0xc4);
