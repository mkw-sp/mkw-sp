#include "SoundArchive.hh"

#include "FileStream.hh"

#include <cstdio>

namespace nw4r::snd {

SoundArchive::~SoundArchive() = default;

void SoundArchive::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~SoundArchive();
    }
}

u32 SoundArchive::getSoundCount() {
    return m_fileReader->getSoundCount();
}

SoundArchive::SoundType SoundArchive::getSoundType(u32 soundId) {
    if (soundId & SASR_BIT) {
        return SoundType::Strm;
    }

    return REPLACED(getSoundType)(soundId);
}

bool SoundArchive::readSoundInfo(u32 soundId, SoundInfo *soundInfo) {
    if (!REPLACED(readSoundInfo)(soundId & ~SASR_BIT, soundInfo)) {
        return false;
    }

    if (soundId & SASR_BIT) {
        soundInfo->fileId = getFileCount() + (soundId & ~SASR_BIT);
    }

    return soundInfo;
}

bool SoundArchive::readStrmSoundInfo(u32 soundId, StrmSoundInfo *strmSoundInfo) {
    if (!(soundId & SASR_BIT)) {
        return REPLACED(readStrmSoundInfo)(soundId, strmSoundInfo);
    }

    switch (REPLACED(getSoundType)(soundId & ~SASR_BIT)) {
    case SoundType::Seq:
    case SoundType::Wave:
        strmSoundInfo->startPosition = 0;
        strmSoundInfo->allocChannelCount = 0; // Use the channel count from the BRSTM header
        strmSoundInfo->allocTrackFlags = 0x1;
        return true;
    default:
        return false;
    }
}

ut::FileStream *SoundArchive::openFileStream(u32 fileId, void *buffer, s32 size) {
    if (fileId & SASR_BIT) {
        if (size < static_cast<s32>(sizeof(FileStream))) {
            return nullptr;
        }

        auto *stream = reinterpret_cast<FileStream *>(fileId);

        auto file = stream->cloneFile();
        if (!file) {
            return nullptr;
        }

        return new (buffer) FileStream(std::move(*file), 0, UINT32_MAX);
    }

    return REPLACED(openFileStream)(fileId, buffer, size);
}

} // namespace nw4r::snd
