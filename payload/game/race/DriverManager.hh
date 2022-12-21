#pragma once

#include <Common.hh>

namespace Race {

class DriverManager {
public:
    void calc();

    static DriverManager *Instance();

private:
    u8 _000[0x1f0 - 0x000];

    static DriverManager *s_instance;
};
static_assert(sizeof(DriverManager) == 0x1f0);

} // namespace Race
