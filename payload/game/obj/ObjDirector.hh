#pragma once

#include <Common.hh>

// TODO: What namespace?
namespace Geo {

// Called ObjManager in Ghidra
class ObjDirector {
public:
    REPLACE void drawDebug();
    void REPLACED(drawDebug)();
};

} // namespace Geo
