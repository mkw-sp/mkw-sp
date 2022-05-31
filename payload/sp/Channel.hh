#pragma once

extern "C" {
#include "Channel.h"
}

#include <egg/core/eggHeap.hh>

namespace SP::Channel {

enum class Status {
    None,
    Older,
    Same,
    Newer,
};

void Init();
Status GetStatus();
void Load(EGG::Heap *heap);
void Unload();
void Install();
void Uninstall();

} // namespace SP::Channel
