#include "NANDArchiveStorage.hh"

extern "C" {
#include <common/Paths.h>
}

#include <cstring>

namespace SP::Storage {

NANDArchiveStorage::NANDArchiveStorage() {
    for (u32 i = 0; i < std::size(m_files); i++) {
        m_files[i].m_storage = this;
    }
    for (u32 i = 0; i < std::size(m_dirs); i++) {
        m_dirs[i].m_storage = this;
    }

    const char *tmpPath = "/tmp/contents.arc";
    const char *path = TITLE_DATA_PATH "/contents.arc";
    if (NANDOpen(tmpPath, &m_fileInfo, NAND_ACCESS_READ) != NAND_RESULT_OK &&
            NANDPrivateOpen(path, &m_fileInfo, NAND_ACCESS_READ) != NAND_RESULT_OK) {
        return;
    }

    alignas(0x20) ARCHeader header;
    if (NANDRead(&m_fileInfo, &header, sizeof(header)) != sizeof(header)) {
        NANDClose(&m_fileInfo);
        return;
    }

    void *metadata = OSAllocFromMEM1ArenaLo(header.fileStart, 0x20);
    if (!metadata) {
        NANDClose(&m_fileInfo);
        return;
    }
    if (NANDSeek(&m_fileInfo, 0, NAND_SEEK_SET) != 0) {
        NANDClose(&m_fileInfo);
        return;
    }
    if (NANDRead(&m_fileInfo, metadata, header.fileStart) != header.fileStart) {
        NANDClose(&m_fileInfo);
        return;
    }

    if (!ARCInitHandle(metadata, &m_handle)) {
        NANDClose(&m_fileInfo);
        return;
    }

    m_ok = true;
}

bool NANDArchiveStorage::ok() const {
    return m_ok;
}

std::optional<FileHandle> NANDArchiveStorage::fastOpen(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (!ARCFastOpen(&m_handle, id, file)) {
        return {};
    }

    file->m_isOpen = true;
    return file;
}

std::optional<FileHandle> NANDArchiveStorage::open(const wchar_t *path, const char *mode) {
    if (strcmp(mode, "r")) {
        return {};
    }

    auto filePath = ConvertPath(path);
    if (!filePath) {
        return {};
    }

    ScopeLock<Mutex> lock(m_mutex);

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (!ARCOpen(&m_handle, filePath->path, file)) {
        return {};
    }

    file->m_isOpen = true;
    return file;
}

bool NANDArchiveStorage::createDir(const wchar_t *UNUSED(path), bool UNUSED(allowNop)) {
    return false;
}

std::optional<DirHandle> NANDArchiveStorage::fastOpenDir(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    if (!ARCFastOpenDir(&m_handle, id, dir)) {
        return {};
    }

    dir->m_isOpen = true;
    return dir;
}

std::optional<DirHandle> NANDArchiveStorage::openDir(const wchar_t *path) {
    auto dirPath = ConvertPath(path);
    if (!dirPath) {
        return {};
    }

    ScopeLock<Mutex> lock(m_mutex);

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    if (!ARCOpenDir(&m_handle, dirPath->path, dir)) {
        return {};
    }

    dir->m_isOpen = true;
    return dir;
}

std::optional<NodeInfo> NANDArchiveStorage::stat(const wchar_t *path) {
    auto nodePath = ConvertPath(path);
    if (!nodePath) {
        return {};
    }

    ScopeLock<Mutex> lock(m_mutex);

    s32 entrynum = ARCConvertPathToEntrynum(&m_handle, nodePath->path);
    if (entrynum < 0) {
        return {};
    }

    NodeInfo info{};
    info.id.storage = this;
    info.id.id = entrynum;
    swprintf(info.name, std::size(info.name), L"%ls", wcsrchr(path, L'/') + 1);

    ARCFileInfo file;
    if (ARCFastOpen(&m_handle, entrynum, &file)) {
        info.type = NodeType::File;
        info.size = ARCGetLength(&file);
        ARCClose(&file);
        return info;
    }

    ARCDir dir;
    if (ARCFastOpenDir(&m_handle, entrynum, &dir)) {
        info.type = NodeType::Dir;
        ARCCloseDir(&dir);
        return info;
    }

    return {};
}

bool NANDArchiveStorage::rename(const wchar_t *UNUSED(srcPath), const wchar_t *UNUSED(dstPath)) {
    return false;
}

bool NANDArchiveStorage::remove(const wchar_t *UNUSED(path), bool UNUSED(allowNop)) {
    return false;
}

std::optional<FileHandle> NANDArchiveStorage::File::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    auto *file = FindNode(m_storage->m_files);
    if (file == std::end(m_storage->m_files)) {
        return {};
    }

    ARCExClone(this, file);
    file->m_isOpen = true;
    return file;
}

bool NANDArchiveStorage::File::close() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!ARCClose(this)) {
        return false;
    }

    m_isOpen = false;
    return true;
}

bool NANDArchiveStorage::File::read(void *dst, u32 size, u32 offset) {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    s32 fileOffset = ARCGetStartOffset(this);
    assert(fileOffset >= 0);
    offset += fileOffset;
    assert(offset < INT32_MAX);
    if (NANDSeek(&m_storage->m_fileInfo, offset, NAND_SEEK_SET) != static_cast<s32>(offset)) {
        return false;
    }

    s32 fileSize = ARCGetLength(this);
    assert(fileSize >= 0);
    if (size > static_cast<u32>(fileSize)) {
        return false;
    }
    assert(size < INT32_MAX);
    return NANDRead(&m_storage->m_fileInfo, dst, size) == static_cast<s32>(size);
}

bool NANDArchiveStorage::File::write(const void *UNUSED(src), u32 UNUSED(size), u32 UNUSED(offset)) {
    return false;
}

bool NANDArchiveStorage::File::sync() {
    return false;
}

u64 NANDArchiveStorage::File::size() {
    return ARCGetLength(this);
}

std::optional<DirHandle> NANDArchiveStorage::Dir::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    auto *dir = FindNode(m_storage->m_dirs);
    if (dir == std::end(m_storage->m_dirs)) {
        return {};
    }

    ARCExCloneDir(this, dir);
    dir->m_isOpen = true;
    return dir;
}

bool NANDArchiveStorage::Dir::close() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!ARCCloseDir(this)) {
        return false;
    }

    m_isOpen = false;
    return true;
}

std::optional<NodeInfo> NANDArchiveStorage::Dir::read() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    ARCDirEntry entry;
    if (!ARCReadDir(this, &entry)) {
        return {};
    }

    NodeInfo info{};
    if (entry.isDir) {
        info.type = NodeType::Dir;
    } else {
        info.type = NodeType::File;
        ARCFileInfo file;
        if (!ARCFastOpen(&m_storage->m_handle, entry.entryNum, &file)) {
            return {};
        }
        info.size = ARCGetLength(&file);
        if (!ARCClose(&file)) {
            return {};
        }
    }
    info.id.storage = m_storage;
    info.id.id = entry.entryNum;
    swprintf(info.name, std::size(info.name), L"%s", entry.name);
    return info;
}

std::optional<NANDArchiveStorage::Path> NANDArchiveStorage::ConvertPath(const wchar_t *path) {
    if (wcsncmp(path, L"ro:/", wcslen(L"ro:/"))) {
        return {};
    }

    Path nodePath;
    snprintf(nodePath.path, std::size(nodePath.path), "%ls", path + wcslen(L"ro:/"));
    return nodePath;
}

} // namespace SP::Storage
