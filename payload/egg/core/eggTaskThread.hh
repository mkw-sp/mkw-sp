#pragma once

#include <Common.hh>

namespace EGG {

template <typename T>
class TaskThread {
public:
    typedef void (*TFunction)(T);

    void request(TFunction mainFunction, T arg, void *taskEndMessage);

private:
    u8 _00[0x58 - 0x00];
};

} // namespace EGG
