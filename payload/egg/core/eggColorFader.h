#pragma once

#include <Common.h>

typedef struct EGG_ColorFader {
    char _00[0x24 - 0x00];
} EGG_ColorFader;

bool EGG_ColorFader_fadeIn(EGG_ColorFader *colorFader);
bool EGG_ColorFader_fadeOut(EGG_ColorFader *colorFader);
