#include "FileStream.hh"

#include <sp/ScopeLock.hh>

#include <iterator>

namespace nw4r::snd {

FileStream::FileStream(SP::Storage::FileHandle file, u32 start, u32 size) : m_file(std::move(file)),
        m_start(start), m_size(size) {}

FileStream::~FileStream() = default;

void FileStream::close() {
    m_file.reset();
    m_cancelled = false;
}

s32 FileStream::read(void *dst, u32 size) {
    if (!m_file) {
        return -1;
    }

    {
        SP::ScopeLock<SP::NoInterrupts> lock;
        while (!OSTryLockMutex(&s_mutex)) {
            OSSleepThread(&s_queue);
            if (m_cancelled) {
                return -1;
            }
        }
    }

    bool result = m_file->read(dst, size, m_start + m_offset);

    {
        SP::ScopeLock<SP::NoInterrupts> lock;
        OSUnlockMutex(&s_mutex);
        OSWakeupThread(&s_queue);
    }

    if (!result) {
        return -1;
    }

    m_offset += size;
    return size;
}

bool FileStream::canAsync() {
    return false;
}

bool FileStream::canRead() {
    return true;
}

bool FileStream::canWrite() {
    return false;
}

u32 FileStream::getOffsetAlign() {
    return 0x20;
}

u32 FileStream::getSizeAlign() {
    return 0x20;
}

u32 FileStream::getBufferAlign() {
    return 0x20;
}

u32 FileStream::getSize() {
    if (!m_file) {
        return 0;
    }

    return std::min(m_file->size(), static_cast<u64>(m_size));
}

void FileStream::seek(s32 offset, SeekOrigin origin) {
    switch (origin) {
    case SeekOrigin::Start:
        m_offset = offset;
        break;
    case SeekOrigin::Current:
        m_offset += offset;
        break;
    case SeekOrigin::End:
        m_offset = getSize() - offset;
        break;
    }
}

void FileStream::cancel() {
    SP::ScopeLock<SP::NoInterrupts> lock;

    m_cancelled = true;
    OSWakeupThread(&s_queue);
}

bool FileStream::canSeek() {
    return true;
}

bool FileStream::canCancel() {
    return true;
}

u32 FileStream::tell() {
    return m_offset;
}

std::optional<SP::Storage::FileHandle> FileStream::cloneFile() {
    if (!m_file) {
        return {};
    }

    return m_file->clone();
}

OSMutex FileStream::s_mutex{};
OSThreadQueue FileStream::s_queue{};

} // namespace nw4r::snd
