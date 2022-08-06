#include "DVDSoundArchive.hh"

#include <sp/ScopeLock.hh>
extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iterator>

namespace nw4r::snd {

DVDSoundArchive::DVDSoundArchive() = default;

DVDSoundArchive::~DVDSoundArchive() = default;

void DVDSoundArchive::dt(s32 type) {
    SoundArchive::dt(type);
}

const void *DVDSoundArchive::getFileAddress(u32 UNUSED(fileId)) {
    return nullptr;
}

const void *DVDSoundArchive::getWaveDataFileAddress(u32 UNUSED(fileId)) {
    return nullptr;
}

size_t DVDSoundArchive::getRequiredStreamBufferSize() {
    return sizeof(FileStream);
}

ut::FileStream *DVDSoundArchive::openStream(void *buffer, s32 size, u32 start, u32 length) {
    if (!m_file) {
        return nullptr;
    }

    if (size < static_cast<s32>(sizeof(FileStream))) {
        return nullptr;
    }

    auto file = m_file->clone();
    if (!file) {
        return nullptr;
    }

    return new (buffer) FileStream(std::move(*file), start, length);
}

ut::FileStream *DVDSoundArchive::openExtStream(void *buffer, s32 size, const char *extFilePath,
        u32 start, u32 UNUSED(length)) {
    if (size < static_cast<s32>(sizeof(FileStream))) {
        return nullptr;
    }

    const char *suffix = strrchr(extFilePath, '.');
    if (suffix && !strcmp(suffix, ".brstm")) {
        suffix = strrchr(extFilePath, '_');
        if (suffix && (!strcmp(suffix, "_f.brstm") || !strcmp(suffix, "_F.brstm"))) {
            if (m_fId) {
                if (auto file = SP::Storage::FastOpen(*m_fId)) {
                    return new (buffer) FileStream(std::move(*file), start, UINT32_MAX);
                }
            }
        }
        m_fId.reset();

        if (auto dir = SP::Storage::OpenRODir(extFilePath)) {
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
                if (auto file = SP::Storage::FastOpen(files[index].id)) {
                    suffix = strrchr(extFilePath, '_');
                    if (suffix && (!strcmp(suffix, "_n.brstm") || !strcmp(suffix, "_N.brstm"))) {
                        char fDirPath[128];
                        snprintf(fDirPath, sizeof(fDirPath), "%s", extFilePath);
                        fDirPath[strlen(fDirPath) - strlen("n.brstm")] -= 'n' - 'f';
                        if (auto fDir = SP::Storage::OpenRODir(fDirPath)) {
                            while (auto fInfo = fDir->read()) {
                                if (fInfo->type != SP::Storage::NodeType::File) {
                                    continue;
                                }
                                if (!wcscmp(fInfo->name, files[index].name)) {
                                    m_fId = fInfo->id;
                                    break;
                                }
                            }
                        }
                    }

                    return new (buffer) FileStream(std::move(*file), start, UINT32_MAX);
                }
            }
        }
    }

    auto file = SP::Storage::OpenRO(extFilePath);
    if (!file) {
        return nullptr;
    }

    return new (buffer) FileStream(std::move(*file), start, UINT32_MAX);
}

bool DVDSoundArchive::open(const char *filePath) {
    m_file = SP::Storage::OpenRO(filePath);
    if (!m_file) {
        return false;
    }

    alignas(0x20) u8 header[0x40];
    if (!m_file->read(header, sizeof(header), 0)) {
        return false;
    }
    m_fileReader.init(header);
    setup(&m_fileReader);

    const char *bare = strrchr(filePath, '/');
    size_t length = bare ? bare - filePath + 1 : strlen(filePath);
    snprintf(m_extFileRoot, sizeof(m_extFileRoot), "%.*s", length, filePath);

    return true;
}

void DVDSoundArchive::close() {
    m_file.reset();
    shutdown();
}

bool DVDSoundArchive::loadHeader(void *buffer, u32 size) {
    if (size < m_fileReader.infoSize()) {
        return false;
    }

    if (!m_file->read(buffer, m_fileReader.infoSize(), m_fileReader.infoOffset())) {
        return false;
    }

    m_fileReader.setInfoChunk(buffer);
    return true;
}

bool DVDSoundArchive::loadLabelStringData(void *buffer, u32 size) {
    if (size < m_fileReader.symbolDataSize()) {
        return false;
    }

    if (!m_file->read(buffer, m_fileReader.symbolDataSize(), m_fileReader.symbolDataOffset())) {
        return false;
    }

    m_fileReader.setStringChunk(buffer);
    return true;
}

DVDSoundArchive::FileStream::FileStream(SP::Storage::FileHandle file, u32 start, u32 size) :
        m_file(std::move(file)), m_start(start), m_size(size) {}

DVDSoundArchive::FileStream::~FileStream() = default;

void DVDSoundArchive::FileStream::close() {
    m_file.reset();
    m_cancelled = false;
}

s32 DVDSoundArchive::FileStream::read(void *dst, u32 size) {
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

bool DVDSoundArchive::FileStream::canAsync() {
    return false;
}

bool DVDSoundArchive::FileStream::canRead() {
    return true;
}

bool DVDSoundArchive::FileStream::canWrite() {
    return false;
}

u32 DVDSoundArchive::FileStream::getOffsetAlign() {
    return 0x20;
}

u32 DVDSoundArchive::FileStream::getSizeAlign() {
    return 0x20;
}

u32 DVDSoundArchive::FileStream::getBufferAlign() {
    return 0x20;
}

u32 DVDSoundArchive::FileStream::getSize() {
    if (!m_file) {
        return 0;
    }

    return std::min(m_file->size(), static_cast<u64>(m_size));
}

void DVDSoundArchive::FileStream::seek(s32 offset, SeekOrigin origin) {
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

void DVDSoundArchive::FileStream::cancel() {
    SP::ScopeLock<SP::NoInterrupts> lock;

    m_cancelled = true;
    OSWakeupThread(&s_queue);
}

bool DVDSoundArchive::FileStream::canSeek() {
    return true;
}

bool DVDSoundArchive::FileStream::canCancel() {
    return true;
}

u32 DVDSoundArchive::FileStream::tell() {
    return m_offset;
}

OSMutex DVDSoundArchive::FileStream::s_mutex{};
OSThreadQueue DVDSoundArchive::FileStream::s_queue{};

} // namespace nw4r::snd
