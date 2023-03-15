#pragma once

#include <Common.hh>

namespace nw4r::ut {

class IOStream {
public:
    virtual ~IOStream();
    virtual void vf_08() = 0; // Not really pure virtual, but we don't need it
    virtual void dt(s32 type);
    virtual void close() = 0;
    virtual s32 read(void *dst, u32 size) = 0; // Same
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();
    virtual void vf_24();
    virtual bool canAsync() = 0;
    virtual bool canRead() = 0;
    virtual bool canWrite() = 0;
    virtual u32 getOffsetAlign() = 0; // Same
    virtual u32 getSizeAlign() = 0;   // Same
    virtual u32 getBufferAlign() = 0; // Same

protected:
    bool m_available;
    u8 _05[0x14 - 0x05];
};
static_assert(sizeof(IOStream) == 0x14);

} // namespace nw4r::ut
