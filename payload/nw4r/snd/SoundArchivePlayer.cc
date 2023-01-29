#include "SoundArchivePlayer.hh"

#include "nw4r/snd/FileStream.hh"

#include <game/sound/PlayerId.hh>
#include <game/system/SaveManager.hh>
#include <sp/storage/Storage.hh>

#include <algorithm>
#include <cwchar>

namespace nw4r::snd {

ut::FileStream **SoundArchivePlayer::soundStreams() {
    u8 *end = reinterpret_cast<u8 *>(m_buffer) + m_bufferSize;
    return reinterpret_cast<ut::FileStream **>(end) - m_archive->getSoundCount();
}

u32 SoundArchivePlayer::getRequiredMemSize(SoundArchive *archive) {
    u32 size = REPLACED(getRequiredMemSize)(archive);
    size += archive->getSoundCount() * sizeof(ut::FileStream *);
    return size;
}

bool SoundArchivePlayer::setupMram(SoundArchive *archive, void *buffer, u32 bufferSize) {
    if (!REPLACED(setupMram)(archive, buffer, bufferSize)) {
        return false;
    }

    std::fill(soundStreams(), soundStreams() + archive->getSoundCount(), nullptr);

    return true;
}

void SoundArchivePlayer::update() {
    auto *saveManager = System::SaveManager::Instance();
    u32 volume = saveManager->getSetting<SP::ClientSettings::Setting::Volume>();
    for (u32 playerId = 0; playerId < m_playerCount; playerId++) {
        m_players[playerId].m_mainOutVolume = volume / 10.0f;
    }
    u32 musicVolume = saveManager->getSetting<SP::ClientSettings::Setting::MusicVolume>();
    m_players[static_cast<u32>(Sound::PlayerId::PL_BGM)].m_mainOutVolume *= musicVolume / 10.0f;

    REPLACED(update)();
}

u32 SoundArchivePlayer::setupSoundImpl(SoundHandle *handle, u32 soundId, void *r6, void *r7,
        bool r8, void *r9) {
    if (soundStreams()[soundId]) {
        soundId |= SASR_BIT;
    }
    return REPLACED(setupSoundImpl)(handle, soundId, r6, r7, r8, r9);
}

u32 SoundArchivePlayer::prepareStrmImpl(void *r4, SoundArchive::SoundInfo *soundInfo, void *r6,
        u32 r7, s32 r8) {
    if (soundInfo->fileId > m_archive->getFileCount()) {
        u32 soundId = soundInfo->fileId - m_archive->getFileCount();
        soundInfo->fileId = reinterpret_cast<u32>(soundStreams()[soundId]);
    }
    u32 result = REPLACED(prepareStrmImpl)(r4, soundInfo, r6, r7, r8);
    return result;
}

bool SoundArchivePlayer::loadGroup(u32 groupId, SoundMemoryAllocatable *allocator, u32 r6) {
    if (auto dir = SP::Storage::OpenRODir("/sound/sasr")) {
        while (auto info = dir->read()) {
            if (info->type != SP::Storage::NodeType::File) {
                continue;
            }

            u32 soundId = 0;
            wchar_t *wcs;
            for (wcs = info->name; wcs - info->name < 9 && L'0' <= *wcs && *wcs <= L'9'; wcs++) {
                soundId = soundId * 10 + (*wcs - L'0');
            }
            if (wcs == info->name) {
                continue;
            }
            if (wcscmp(wcs, L".brstm")) {
                continue;
            }

            switch (m_archive->getSoundType(soundId)) {
            case SoundArchive::SoundType::Seq:
            case SoundArchive::SoundType::Wave:
                break;
            default:
                continue;
            }

            if (soundStreams()[soundId]) {
                continue;
            }

            SoundArchive::SoundInfo soundInfo;
            if (!m_archive->readSoundInfo(soundId, &soundInfo)) {
                continue;
            }

            SoundArchive::FileInfo fileInfo;
            if (!m_archive->readFileInfo(soundInfo.fileId, &fileInfo)) {
                continue;
            }

            for (u32 i = 0; i < fileInfo.filePosCount; i++) {
                SoundArchive::FilePos filePos;
                if (!m_archive->readFilePos(soundInfo.fileId, i, &filePos)) {
                    continue;
                }

                if (filePos.groupId != groupId) {
                    continue;
                }

                if (auto file = SP::Storage::FastOpen(info->id)) {
                    void *buffer = allocator->alloc(sizeof(FileStream));
                    auto *stream = new (buffer) FileStream(std::move(*file), 0, UINT32_MAX);
                    soundStreams()[soundId] = stream;
                }

                break;
            }
        }
    }

    return REPLACED(loadGroup)(groupId, allocator, r6);
}

void SoundArchivePlayer::invalidateData(const void *start, const void *end) {
    REPLACED(invalidateData)(start, end);

    for (u32 soundId = 0; soundId < m_archive->getSoundCount(); soundId++) {
        if (start <= soundStreams()[soundId] && soundStreams()[soundId] <= end) {
            soundStreams()[soundId]->~FileStream();
            soundStreams()[soundId] = nullptr;
        }
    }
}

} // namespace nw4r::snd
