#pragma once

namespace System {

enum class ContextId {
    None = -1,
    System = 0,
    Game = 1,
    Channel = 2,
    Trial = 4,
    SP = 5, // Added
};

} // namespace System
