#pragma once

#include <Common.hh>

namespace Item {

class ItemManager {
public:
    void calc();

    static ItemManager *Instance();

private:
    u8 _000[0x430 - 0x000];

    static ItemManager *s_instance;
};
static_assert(sizeof(ItemManager) == 0x430);

} // namespace Item
