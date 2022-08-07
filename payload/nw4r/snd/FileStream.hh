#pragma once

#include "nw4r/ut/FileStream.hh"

extern "C" {
#include <revolution.h>
}
#include <sp/storage/Storage.hh>

namespace nw4r::snd {

class FileStream : public ut::FileStream {
public:
    FileStream(SP::Storage::FileHandle file, u32 start, u32 size);
    ~FileStream() override;
    void close() override;
    s32 read(void *dst, u32 size) override;
    bool canAsync() override;
    bool canRead() override;
    bool canWrite() override;
    u32 getOffsetAlign() override;
    u32 getSizeAlign() override;
    u32 getBufferAlign() override;
    u32 getSize() override;
    void seek(s32 offset, SeekOrigin origin) override;
    void cancel() override;
    bool canSeek() override;
    bool canCancel() override;
    u32 tell() override;

    std::optional<SP::Storage::FileHandle> cloneFile();

private:
    std::optional<SP::Storage::FileHandle> m_file;
    u32 m_start;
    u32 m_size;
    u32 m_offset = 0;
    bool m_cancelled = false;

    static OSMutex s_mutex;
    static OSThreadQueue s_queue;
};
static_assert(sizeof(FileStream) <= 0x100);

} // namespace nw4r::snd
