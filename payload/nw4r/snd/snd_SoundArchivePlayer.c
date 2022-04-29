#include "snd_SoundArchivePlayer.h"

#include "nw4r/snd/snd_SoundArchiveLoader.h"

static bool snd_SoundArchivePlayer_LoadFile(snd_SoundArchivePlayer *self,
        snd_SoundArchiveLoader *loader, u32 fileId, void *r6) {
    if (fileId >= snd_SoundArchive_GetFileCount(self->archive)) {
        return false;
    }

    if (!self->fileTable->entries[fileId].address) {
        void *address = snd_SoundArchiveLoader_LoadFile(loader, fileId, r6);
        if (!address) {
            return false;
        }
        self->fileTable->entries[fileId].address = address;
    }

    snd_FileInfo fileInfo;
    if (!snd_SoundArchive_ReadFileInfo(self->archive, fileId, &fileInfo)) {
        return false;
    }
    if (fileInfo.waveDataFileSize > 0 && !self->fileTable->entries[fileId].waveDataAddress) {
        void *waveDataAddress = snd_SoundArchiveLoader_LoadWaveDataFile(loader, fileId, r6);
        if (!waveDataAddress) {
            return false;
        }
        self->fileTable->entries[fileId].waveDataAddress = waveDataAddress;
    }

    return true;
}

static bool my_snd_SoundArchivePlayer_LoadGroup(snd_SoundArchivePlayer *self, u32 groupId, void *r5,
        u32 UNUSED(r6)) {
    if (!snd_SoundArchivePlayer_IsAvailable(self)) {
        return false;
    }

    if (groupId >= snd_SoundArchive_GetGroupCount(self->archive)) {
        return false;
    }

    // The destructor is a stub
    snd_SoundArchiveLoader loader;
    snd_SoundArchiveLoader_ct(&loader, self->archive);

    snd_GroupInfo groupInfo;
    if (!snd_SoundArchive_ReadGroupInfo(self->archive, groupId, &groupInfo)) {
        return false;
    }
    for (u32 i = 0; i < groupInfo.itemCount; i++) {
        snd_GroupItemInfo groupItemInfo;
        if (!snd_SoundArchive_ReadGroupItemInfo(self->archive, groupId, i, &groupItemInfo)) {
            return false;
        }
        u32 fileId = groupItemInfo.fileId;

        if (!snd_SoundArchivePlayer_LoadFile(self, &loader, fileId, r5)) {
            return false;
        }
    }

    return true;
}
PATCH_B(snd_SoundArchivePlayer_LoadGroup, my_snd_SoundArchivePlayer_LoadGroup);

static void *my_snd_SoundArchivePlayer_GetFileAddress(snd_SoundArchivePlayer *self, u32 fileId) {
    return self->fileTable->entries[fileId].address;
}
PATCH_B(snd_SoundArchivePlayer_GetFileAddress, my_snd_SoundArchivePlayer_GetFileAddress);

static void *my_snd_SoundArchivePlayer_GetFileWaveDataAddress(snd_SoundArchivePlayer *self,
        u32 fileId) {
    return self->fileTable->entries[fileId].waveDataAddress;
}
PATCH_B(snd_SoundArchivePlayer_GetFileWaveDataAddress,
        my_snd_SoundArchivePlayer_GetFileWaveDataAddress);
