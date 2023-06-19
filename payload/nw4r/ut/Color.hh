#pragma once

extern "C" {
#include "revolution/gx/GXStruct.h"
}

namespace nw4r::ut {

struct Color : public GXColor {
public:
    static constexpr u32 Red = 0xFF0000FF;
    static constexpr u32 Green = 0x00FF00FF;
    static constexpr u32 Blue = 0x0000FFFF;
    static constexpr u32 White = 0xFFFFFFFF;
    static constexpr u32 Black = 0x000000FF;

    Color(u8 r, u8 g, u8 b, u8 a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color(u32 color) {
        this->r = (color & 0xFF000000) >> 24;
        this->g = (color & 0x00FF0000) >> 16;
        this->b = (color & 0x0000FF00) >> 8;
        this->a = (color & 0x000000FF) >> 0;
    }
};

} // namespace nw4r::ut
