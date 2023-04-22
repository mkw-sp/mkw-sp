#pragma once

#include <Common.hh>

namespace System {

enum class NandResult : u32 {
    Ok = 0x0,
    Busy = 0x2,
    Access = 0x3,
    NoAccess = 0x4,
    NoSpace = 0x7,
    Other = 0x8,
};

}
