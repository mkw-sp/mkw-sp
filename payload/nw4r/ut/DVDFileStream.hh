#pragma once

#include "nw4r/ut/IOStream.hh"

#include <sp/storage/Storage.hh>

namespace nw4r::ut {

class DVDFileStream : public IOStream {
    ~DVDFileStream() override;
    void vf_08() override;
    REPLACE void dt(s32 type) override;
    REPLACE void close() override;
    REPLACE s32 read(void *dst, u32 size) override;
    void vf_18() override;
    void vf_24() override;
    REPLACE bool canAsync() override;
    void vf_2c() override;
    void vf_30() override;
    void vf_34() override;
    void vf_38() override;
    void vf_3c() override;
    REPLACE u32 getSize() override;
    REPLACE void seek(u32 offset) override;
    void vf_48() override;
    void vf_4c() override;
    void vf_50() override;
    REPLACE bool canCancel() override;
    REPLACE u32 tell() override;
    void vf_5c() override;
    void vf_60() override;

private:
    u8 _14[0x28 - 0x14];
    std::optional<FileHandle 
    u8 _1c[0x70 - 0x1c];
};
static_assert(sizeof(DVDFileStream) == 0x70);

} // namespace nw4r::ut
