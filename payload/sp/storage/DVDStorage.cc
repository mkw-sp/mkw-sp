#include "DVDStorage.hh"

#include <Common.hh>

#include <cstring>

namespace SP::Storage {

DVDStorage::DVDStorage() {
    for (u32 i = 0; i < std::size(m_files); i++) {
        m_files[i].m_storage = this;
    }
    for (u32 i = 0; i < std::size(m_dirs); i++) {
        m_dirs[i].m_storage = this;
    }
}

std::optional<FileHandle> DVDStorage::fastOpen(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (!DVDFastOpen(id, file)) {
        return {};
    }

    file->m_isOpen = true;
    return file;
}

std::optional<FileHandle> DVDStorage::open(const wchar_t *path, const char *mode) {
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

    if (!DVDOpen(filePath->path, file)) {
        return {};
    }

    file->m_isOpen = true;
    return file;
}

bool DVDStorage::createDir(const wchar_t * /* path */, bool /* allowNop */) {
    return false;
}

std::optional<DirHandle> DVDStorage::fastOpenDir(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    if (!DVDFastOpenDir(id, dir)) {
        return {};
    }

    dir->m_isOpen = true;
    return dir;
}

std::optional<DirHandle> DVDStorage::openDir(const wchar_t *path) {
    auto dirPath = ConvertPath(path);
    if (!dirPath) {
        return {};
    }

    ScopeLock<Mutex> lock(m_mutex);

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    if (!DVDOpenDir(dirPath->path, dir)) {
        return {};
    }

    dir->m_isOpen = true;
    return dir;
}

std::optional<NodeInfo> DVDStorage::stat(const wchar_t *path) {
    auto nodePath = ConvertPath(path);
    if (!nodePath) {
        return {};
    }

    ScopeLock<Mutex> lock(m_mutex);

    s32 entrynum = DVDConvertPathToEntrynum(nodePath->path);
    if (entrynum < 0) {
        return {};
    }

    NodeInfo info{};
    info.id.storage = this;
    info.id.id = entrynum;
    swprintf(info.name, std::size(info.name), L"%ls", wcsrchr(path, L'/') + 1);

    DVDFileInfo file;
    if (DVDFastOpen(entrynum, &file)) {
        info.type = NodeType::File;
        info.size = AlignUp(file.length, 32);
        ;
        DVDClose(&file);
        return info;
    }

    DVDDir dir;
    if (DVDFastOpenDir(entrynum, &dir)) {
        info.type = NodeType::Dir;
        DVDCloseDir(&dir);
        return info;
    }

    return {};
}

bool DVDStorage::rename(const wchar_t * /* srcPath */, const wchar_t * /* dstPath */) {
    return false;
}

bool DVDStorage::remove(const wchar_t * /* path */, bool /* allowNop */) {
    return false;
}

std::optional<FileHandle> DVDStorage::startBenchmark() {
    return {};
}

void DVDStorage::endBenchmark() {}

u32 DVDStorage::getMessageId() {
    return 0;
}

std::optional<FileHandle> DVDStorage::File::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    auto *file = FindNode(m_storage->m_files);
    if (file == std::end(m_storage->m_files)) {
        return {};
    }

    DVDExClone(this, file);
    file->m_isOpen = true;
    return file;
}

bool DVDStorage::File::close() {
    if (!DVDClose(this)) {
        return false;
    }

    m_isOpen = false;
    return true;
}

bool DVDStorage::File::read(void *dst, u32 size, u32 offset) {
    assert((reinterpret_cast<size_t>(dst) & 0x1f) == 0);
    assert((size & 0x1f) == 0);
    assert((offset & 0x1f) == 0);

    return DVDRead(this, dst, size, offset);
}

bool DVDStorage::File::write(const void * /* src */, u32 /* size */, u32 /* offset */) {
    return false;
}

bool DVDStorage::File::sync() {
    return false;
}

u64 DVDStorage::File::size() {
    return AlignUp(this->length, 32);
}

std::optional<DirHandle> DVDStorage::Dir::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    auto *dir = FindNode(m_storage->m_dirs);
    if (dir == std::end(m_storage->m_dirs)) {
        return {};
    }

    DVDExCloneDir(this, dir);
    dir->m_isOpen = true;
    return dir;
}

bool DVDStorage::Dir::close() {
    if (!DVDCloseDir(this)) {
        return false;
    }

    m_isOpen = false;
    return true;
}

std::optional<NodeInfo> DVDStorage::Dir::read() {
    DVDDirEntry entry;
    if (!DVDReadDir(this, &entry)) {
        return {};
    }

    NodeInfo info{};
    if (entry.isDir) {
        info.type = NodeType::Dir;
    } else {
        info.type = NodeType::File;
        DVDFileInfo file;
        if (!DVDFastOpen(entry.entryNum, &file)) {
            return {};
        }
        info.size = AlignUp(file.length, 32);
        if (!DVDClose(&file)) {
            return {};
        }
    }
    info.id.storage = m_storage;
    info.id.id = entry.entryNum;
    swprintf(info.name, std::size(info.name), L"%s", entry.name);
    return info;
}

std::optional<DVDStorage::Path> DVDStorage::ConvertPath(const wchar_t *path) {
    if (wcsncmp(path, L"ro:/", wcslen(L"ro:/"))) {
        return {};
    }

    Path nodePath;
    snprintf(nodePath.path, std::size(nodePath.path), "%ls", path + wcslen(L"ro:/"));
    return nodePath;
}

} // namespace SP::Storage
