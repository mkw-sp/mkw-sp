#pragma once

#include "nw4r/ut/IOStream.hh"

namespace nw4r::ut {

class FileStream : public IOStream {
public:
    enum class SeekOrigin {
        Start,
        Current,
        End,
    };

    virtual ~FileStream() override;
    virtual void vf_08() override;
    virtual u32 getSize() = 0;
    // Not really pure virtual, but we don't need it
    virtual void seek(s32 offset, SeekOrigin origin) = 0;
    virtual void cancel();
    virtual bool cancelAsync(void *r4, void *r5);
    virtual bool canSeek() = 0;
    virtual bool canCancel() = 0;
    virtual u32 tell() = 0;
};

} // namespace nw4r::ut
