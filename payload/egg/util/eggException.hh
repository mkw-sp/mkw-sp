#pragma once

#include "egg/core/eggHeap.hh"

#include <nw4r/db/dbConsole.hh>

namespace EGG {

REPLACE bool ExceptionCallBack_(nw4r::db::ConsoleHandle console, void *arg);

class Exception {
public:
    virtual ~Exception();

    static Exception *REPLACED(
            create)(u16 width, u16 height, u16 attributes, Heap *heap, u32 numMapFiles);
    REPLACE static Exception *create(u16 width, u16 height, u16 attributes, Heap *heap,
            u32 numMapFiles);
    static void REPLACED(setUserCallBack)(void *arg);
    REPLACE static void setUserCallBack(void *arg);
};
static_assert(sizeof(Exception) == 0x4);

} // namespace EGG
