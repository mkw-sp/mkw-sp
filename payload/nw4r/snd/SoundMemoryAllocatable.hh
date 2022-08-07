#pragma once

namespace nw4r::snd {

class SoundMemoryAllocatable {
public:
    virtual ~SoundMemoryAllocatable();
    virtual void dt(s32 type);
    virtual void *alloc(u32 size);
};

} // namespace nw4r::snd
