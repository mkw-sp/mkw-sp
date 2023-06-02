#pragma once

#include <nw4r/lyt/lyt_layout.h>

typedef struct {
    float proj_mtx[4][4];
    float width;
    float height;
    float pos_mtx[3][4];
} ScreenDrawData;
typedef struct {
    float cursor_x;
    float cursor_y;
    float max_height; // For the current line
    Rect base_region;
} WriterTextBox;
typedef struct {
    WriterTextBox mBox;
    ScreenDrawData mScreen;
    void *mLastTexObj;
} TextWriter;

void TextWriter_configure(TextWriter *self, Rect box, void * /* res */);
void TextWriter_reset(TextWriter *self);
void TextWriter_beginDraw(TextWriter *self);
void TextWriter_draw(TextWriter *self, const char *str, u32 len, float size, u8 alpha_override);
void TextWriter_endDraw(TextWriter *self);
