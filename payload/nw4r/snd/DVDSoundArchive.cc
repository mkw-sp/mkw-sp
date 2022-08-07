#include "DVDSoundArchive.hh"

#include "nw4r/snd/FileStream.hh"

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

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

} // namespace nw4r::snd
