#pragma once

#include <Common.hh>

// TODO: What namespace?
namespace Geo {

// Called ObjManager in Ghidra
class ObjDirector {
public:
    void calc();
    void calcBT();
    REPLACE void drawDebug();
    void REPLACED(drawDebug)();

    static ObjDirector *Instance();

private:
    u8 _000[0x160 - 0x000];

    static ObjDirector *s_instance;
};

} // namespace Geo
