#pragma once

#include <Common.hh>

namespace Battle {

class CoinManager {
public:
    void calcScreens();

    static CoinManager *Instance();

private:
    u8 _0000[0x43b4 - 0x0000];

    static CoinManager *s_instance;
};
static_assert(sizeof(CoinManager) == 0x43b4);

} // namespace Battle
