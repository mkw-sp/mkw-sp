#pragma once

#include <common/IOS.hh>

namespace IOS {

class Dolphin final : private Resource {
public:
    Dolphin();
    ~Dolphin();
    using Resource::ok;

    bool setSpeedLimit(u32 speedLimit);
};

} // namespace IOS
