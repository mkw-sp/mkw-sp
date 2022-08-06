#include "SoundArchive.hh"

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

#if ENABLE_SASR
// With the changes in SoundArchivePlayer, the 3 remaining uses of that function in the base game
// all need the file count instead. And in loadGroup we simply use the REPLACED macro.
u32 SoundArchive::getGroupCount() {
    return getFileCount();
}

ut::FileStream *SoundArchive::openFileStream(u32 fileId, void *buffer, s32 size) {
    FileInfo fileInfo;
    if (!readFileInfo(fileId, &fileInfo)) {
        return nullptr;
    }
    u32 length = fileInfo.fileSize;

    char path[256];
    if (fileInfo.extFilePath) {
        if (fileInfo.extFilePath[0] == '/') {
            snprintf(path, sizeof(path), "%s", fileInfo.extFilePath);
        } else {
            snprintf(path, sizeof(path), "%s%s", m_extFileRoot, fileInfo.extFilePath);
        }
        return openExtStream(buffer, size, path, 0, length);
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
            snprintf(path, sizeof(path), "%s/%u.%s", prefixes[i], fileId, exts[i]);
            auto *stream = openExtStream(buffer, size, path, 0, length);
            if (stream) {
                return stream;
            }
        }
    }

    FilePos filePos;
    if (!readFilePos(fileId, 0, &filePos)) {
        return nullptr;
    }

    GroupInfo groupInfo;
    if (!readGroupInfo(filePos.groupId, &groupInfo)) {
        return nullptr;
    }

    GroupItemInfo groupItemInfo;
    if (!readGroupItemInfo(filePos.groupId, filePos.index, &groupItemInfo)) {
        return nullptr;
    }

    u32 offset = groupInfo.offset + groupItemInfo.offset;
    return openStream(buffer, size, offset, length);
}

ut::FileStream *SoundArchive::openFileStreamWaveData(u32 fileId, void *buffer, s32 size) {
    FileInfo fileInfo;
    if (!readFileInfo(fileId, &fileInfo)) {
        return nullptr;
    }
    u32 length = fileInfo.waveDataFileSize;

    char path[256];
    snprintf(path, sizeof(path), "/sound/wave/%u.brwar", fileId);
    auto *stream = openExtStream(buffer, size, path, 0, length);
    if (stream) {
        return stream;
    }
    FilePos filePos;
    if (!readFilePos(fileId, 0, &filePos)) {
        return nullptr;
    }

    GroupInfo groupInfo;
    if (!readGroupInfo(filePos.groupId, &groupInfo)) {
        return nullptr;
    }

    GroupItemInfo groupItemInfo;
    if (!readGroupItemInfo(filePos.groupId, filePos.index, &groupItemInfo)) {
        return nullptr;
    }

    u32 offset = groupInfo.waveDataOffset + groupItemInfo.waveDataOffset;
    return openStream(buffer, size, offset, length);
}
#endif

} // namespace nw4r::snd
