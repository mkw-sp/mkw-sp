#include "SoundArchivePlayer.hh"

namespace nw4r::snd {

bool SoundArchivePlayer::loadGroup(u32 groupId, void *r5, u32 UNUSED(r6)) {
    if (!isAvailable()) {
        return false;
    }

    if (groupId >= m_archive->REPLACED(getGroupCount)()) {
        return false;
    }

    SoundArchiveLoader loader(m_archive);

    SoundArchive::GroupInfo groupInfo;
    if (!m_archive->readGroupInfo(groupId, &groupInfo)) {
        return false;
    }
    for (u32 i = 0; i < groupInfo.itemCount; i++) {
        SoundArchive::GroupItemInfo groupItemInfo;
        if (!m_archive->readGroupItemInfo(groupId, i, &groupItemInfo)) {
            return false;
        }
        u32 fileId = groupItemInfo.fileId;

        if (!loadFile(&loader, fileId, r5)) {
            return false;
        }
    }

    return true;
}

bool SoundArchivePlayer::loadFile(SoundArchiveLoader *loader, u32 fileId, void *r6) {
    if (fileId >= m_archive->getFileCount()) {
        return false;
    }

    if (!m_fileTable->entries[fileId].address) {
        void *address = loader->loadFile(fileId, r6);
        if (!address) {
            return false;
        }
        m_fileTable->entries[fileId].address = address;
    }

    SoundArchive::FileInfo fileInfo;
    if (!m_archive->readFileInfo(fileId, &fileInfo)) {
        return false;
    }
    if (fileInfo.waveDataFileSize > 0 && !m_fileTable->entries[fileId].waveDataAddress) {
        void *waveDataAddress = loader->loadWaveDataFile(fileId, r6);
        if (!waveDataAddress) {
            return false;
        }
        m_fileTable->entries[fileId].waveDataAddress = waveDataAddress;
    }

    return true;
}

void *SoundArchivePlayer::getFileAddress(u32 fileId) {
    return m_fileTable->entries[fileId].address;
}

void *SoundArchivePlayer::getFileWaveDataAddress(u32 fileId) {
    return m_fileTable->entries[fileId].waveDataAddress;
}

} // namespace nw4r::snd
