#pragma once

#include <Common.hh>

namespace Race {

class BoxColManager {
public:
    void calc();

    static BoxColManager *Instance();

private:
    u8 _000[0x454 - 0x000];

    static BoxColManager *s_instance;
};
static_assert(sizeof(BoxColManager) == 0x454);

} // namespace Race
