#pragma once

#include <Common.hh>

namespace Render {

// Tentative name. DrawList in Ghidra, RenderManager in IDA
class DrawList {
public:
    static DrawList *spInstance;

    std::array<float, 12> &getViewMatrix() {
        return _10->_6C->getViewMatrix();
    }

private:
    struct Struct_0x6c {
        virtual void vf00() = 0;
        virtual void vf04() = 0;
        virtual std::array<float, 12> &getViewMatrix() = 0;
        // ...
    };
    struct Struct_0x10 {
        char _00[0x6c - 0x00];
        Struct_0x6c *_6C;
        // ...
    };
    char _00[0x10 - 0x00];
    Struct_0x10 *_10;
    // ...
};

} // namespace Render
