#pragma once

#include <Common.h>

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;
static_assert(sizeof(GXColor) == 0x4);

#define GXCOLOR_RED   (GXColor){ 0xFF, 0x00, 0x00, 0xFF }
#define GXCOLOR_GREEN (GXColor){ 0x00, 0xFF, 0x00, 0xFF }
#define GXCOLOR_BLUE  (GXColor){ 0x00, 0x00, 0xFF, 0xFF }
#define GXCOLOR_PINK  (GXColor){ 0xFF, 0x00, 0xFF, 0xFF }
#define GXCOLOR_WHITE (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF }
#define GXCOLOR_BLACK (GXColor){ 0x00, 0x00, 0x00, 0xFF }

typedef struct {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10;
static_assert(sizeof(GXColorS10) == 0x8);

typedef struct GXRenderModeObj {
    int tv_mode;
    u16 fb_width;
    u16 efb_height;
    u16 xfb_height;
    u16 vi_x;
    u16 vi_y;
    u16 vi_width;
    u16 vi_height;
    int vi_xfb;
    u8 field;
    u8 aa;
    u8 sample[12][2];
    u8 vert_filter[7];
} GXRenderModeObj;
static_assert(sizeof(GXRenderModeObj) == 0x3c);

typedef struct {
    u8 _00[0x20 - 0x00];
} GXTexObj;
