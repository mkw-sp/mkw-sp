#pragma once

#include <Common.hh>

namespace EGG {

class TaskThread {
public:
    typedef void (*TFunction)(void *);

    void request(TFunction mainFunction, void *arg, void *taskEndMessage);

private:
    u8 _00[0x58 - 0x00];
};

} // namespace EGG
