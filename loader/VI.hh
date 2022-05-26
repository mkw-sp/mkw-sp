#pragma once

#include <Common.hh>

namespace VI {

void Init();

u16 GetXFBWidth();

u16 GetXFBHeight();

u8 ReadGrayscaleFromXFB(u16 x, u16 y);

void WriteGrayscaleToXFB(u16 x, u16 y, u8 intensity);

void FlushXFB();

} // namespace VI
