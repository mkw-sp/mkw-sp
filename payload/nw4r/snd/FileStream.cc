#include "FileStream.hh"

#include <sp/ScopeLock.hh>

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

#include <cstring>
#include <iterator>

namespace nw4r::snd {

FileStream::FileStream(const char *path) {
    m_path.emplace();
    if (snprintf(m_path->data(), m_path->size(), "%s", path) >= static_cast<s32>(m_path->size())) {
        m_path.reset();
    }
}

FileStream::FileStream(SP::Storage::FileHandle file, u32 start, u32 size)
    : m_file(std::move(file)), m_start(start), m_size(size) {}

FileStream::~FileStream() = default;

void FileStream::close() {
    m_path.reset();
    m_file.reset();
    m_cancelled = false;
}

s32 FileStream::read(void *dst, u32 size) {
    {
        SP::ScopeLock<SP::NoInterrupts> lock;
        while (!OSTryLockMutex(&s_mutex)) {
            OSSleepThread(&s_queue);
            if (m_cancelled) {
                return -1;
            }
        }
    }

    if (!open()) {
        return -1;
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
    if (!open()) {
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
    if (!open()) {
        return {};
    }

    return m_file->clone();
}

bool FileStream::open() {
    if (m_file) {
        return true;
    }

    if (!m_path) {
        return false;
    }

    auto path = *m_path;
    m_path.reset();

    const char *suffix = strrchr(path.data(), '.');
    if (suffix && !strcmp(suffix, ".brstm")) {
        suffix = strrchr(path.data(), '_');
        if (suffix && (!strcmp(suffix, "_f.brstm") || !strcmp(suffix, "_F.brstm"))) {
            if (s_fId) {
                if (m_file = SP::Storage::FastOpen(*s_fId)) {
                    return true;
                }
            }
        }
        s_fId.reset();

        if (auto dir = SP::Storage::OpenRODir(path.data())) {
            SP::Storage::NodeInfo files[8];
            u32 count = 0;
            while (auto info = dir->read()) {
                if (info->type == SP::Storage::NodeType::File) {
                    files[count++] = *info;
                    if (count == std::size(files)) {
                        break;
                    }
                }
            }
            if (count > 0) {
                u32 index = hydro_random_uniform(count);
                if (m_file = SP::Storage::FastOpen(files[index].id)) {
                    suffix = strrchr(path.data(), '_');
                    if (suffix && (!strcmp(suffix, "_n.brstm") || !strcmp(suffix, "_N.brstm"))) {
                        char fDirPath[128];
                        snprintf(fDirPath, sizeof(fDirPath), "%s", path.data());
                        fDirPath[strlen(fDirPath) - strlen("n.brstm")] -= 'n' - 'f';
                        if (auto fDir = SP::Storage::OpenRODir(fDirPath)) {
                            while (auto fInfo = fDir->read()) {
                                if (fInfo->type != SP::Storage::NodeType::File) {
                                    continue;
                                }
                                if (!wcscmp(fInfo->name, files[index].name)) {
                                    s_fId = fInfo->id;
                                    break;
                                }
                            }
                        }
                    }

                    return true;
                }
            }
        }
    }

    m_file = SP::Storage::OpenRO(path.data());
    return m_file.has_value();
}

OSMutex FileStream::s_mutex{};
OSThreadQueue FileStream::s_queue{};
std::optional<SP::Storage::NodeId> FileStream::s_fId{};

} // namespace nw4r::snd
