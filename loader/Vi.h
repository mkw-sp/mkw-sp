#pragma once

#include <Common.h>

void Vi_init(void);

u16 Vi_getXfbWidth(void);

u16 Vi_getXfbHeight(void);

u8 Vi_readGrayscaleFromXfb(u16 x, u16 y);

void Vi_writeGrayscaleToXfb(u16 x, u16 y, u8 intensity);

void Vi_flushXfb(void);
