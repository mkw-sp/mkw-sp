#include "snd_SoundArchive.h"

#include <stdio.h>

static ut_FileStream *my_snd_SoundArchive_OpenFileStream(snd_SoundArchive *self, u32 fileId,
        void *buffer, s32 size) {
    snd_FileInfo fileInfo;
    if (!snd_SoundArchive_ReadFileInfo(self, fileId, &fileInfo)) {
        return NULL;
    }
    u32 length = fileInfo.fileSize;

    char path[256];
    if (fileInfo.extFilePath) {
        if (fileInfo.extFilePath[0] == '/') {
            snprintf(path, sizeof(path), "%s", fileInfo.extFilePath);
        } else {
            snprintf(path, sizeof(path), "%s%s", self->extFileRoot, fileInfo.extFilePath);
        }
        return self->vt->OpenExtStream(self, buffer, size, path, 0, length);
    } else {
        const char *prefixes[] = {
            "/sound/seq",
            "/sound/wsd",
            "/sound/bank",
        };
        const char *exts[] = {
            "brseq",
            "brwsd",
            "brbnk",
        };
        for (u32 i = 0; i < 3; i++) {
            snprintf(path, sizeof(path), "%s/%lu.%s", prefixes[i], fileId, exts[i]);
            ut_FileStream *stream = self->vt->OpenExtStream(self, buffer, size, path, 0, length);
            if (stream) {
                return stream;
            }
        }
    }

    snd_FilePos filePos;
    if (!snd_SoundArchive_ReadFilePos(self, fileId, 0, &filePos)) {
        return NULL;
    }

    snd_GroupInfo groupInfo;
    if (!snd_SoundArchive_ReadGroupInfo(self, filePos.groupId, &groupInfo)) {
        return NULL;
    }

    snd_GroupItemInfo groupItemInfo;
    if (!snd_SoundArchive_ReadGroupItemInfo(self, filePos.groupId, filePos.index, &groupItemInfo)) {
        return NULL;
    }

    u32 offset = groupInfo.offset + groupItemInfo.offset;
    return self->vt->OpenStream(self, buffer, size, offset, length);
}
PATCH_B(snd_SoundArchive_OpenFileStream, my_snd_SoundArchive_OpenFileStream);

static ut_FileStream *my_snd_SoundArchive_OpenFileWaveDataStream(snd_SoundArchive *self, u32 fileId,
        void *buffer, s32 size) {
    snd_FileInfo fileInfo;
    if (!snd_SoundArchive_ReadFileInfo(self, fileId, &fileInfo)) {
        return NULL;
    }
    u32 length = fileInfo.waveDataFileSize;

    char path[256];
    snprintf(path, sizeof(path), "/sound/wave/%lu.brwar", fileId);
    ut_FileStream *stream = self->vt->OpenExtStream(self, buffer, size, path, 0, length);
    if (stream) {
        return stream;
    }
    snd_FilePos filePos;
    if (!snd_SoundArchive_ReadFilePos(self, fileId, 0, &filePos)) {
        return NULL;
    }

    snd_GroupInfo groupInfo;
    if (!snd_SoundArchive_ReadGroupInfo(self, filePos.groupId, &groupInfo)) {
        return NULL;
    }

    snd_GroupItemInfo groupItemInfo;
    if (!snd_SoundArchive_ReadGroupItemInfo(self, filePos.groupId, filePos.index, &groupItemInfo)) {
        return NULL;
    }

    u32 offset = groupInfo.waveDataOffset + groupItemInfo.waveDataOffset;
    return self->vt->OpenStream(self, buffer, size, offset, length);
}
PATCH_B(snd_SoundArchive_OpenFileWaveDataStream, my_snd_SoundArchive_OpenFileWaveDataStream);
