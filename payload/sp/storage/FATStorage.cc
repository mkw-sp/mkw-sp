#include "FATStorage.hh"

#include "sp/settings/FileReplacement.hh"
#include "sp/settings/GlobalSettings.hh"

extern "C" {
#include "sp/storage/Sdi.h"
#include "sp/storage/UsbStorage.h"
}

#include <array>
#include <cstring>

namespace SP::Storage {

OSTime FATStorage::ConvertTimeToTicks(u16 date, u16 time) {
    OSCalendarTime calendarTime = {};
    calendarTime.sec = (time & 0x1F) << 1;
    calendarTime.min = (time >> 5) & 0x3F;
    calendarTime.hour = time >> 11;
    calendarTime.mday = date & 0x1F;
    calendarTime.mon = ((date >> 5) & 0x0F) - 1;
    calendarTime.year = (date >> 9) + 1980;

    return OSCalendarTimeToTicks(&calendarTime);
}

FATStorage::FATStorage() {
    if (s_storage) {
        return;
    }

    for (u32 i = 0; i < std::size(m_files); i++) {
        m_files[i].m_storage = this;
    }
    for (u32 i = 0; i < std::size(m_dirs); i++) {
        m_dirs[i].m_storage = this;
    }

    std::array initFuncs{SdiStorage_init, UsbStorage_init};

    for (auto initFunc : initFuncs) {
        if (!initFunc(&s_storage)) {
            SP_LOG("Failed to initialize the device");
            continue;
        }

        if (f_mount(&m_fs, L"", 1) != FR_OK) {
            SP_LOG("Failed to mount the filesystem");
            continue;
        }

        FRESULT result = f_mkdir(L"/mkw-sp");
        if (result != FR_OK && result != FR_EXIST) {
            SP_LOG("Failed to create or open the /mkw-sp directory");
            continue;
        }

        if (f_chdir(L"/mkw-sp") != FR_OK) {
            SP_LOG("Failed to change the current directory to /mkw-sp");
            continue;
        }

        auto dir = openDir(L"/mkw-sp");
        if (!dir) {
            SP_LOG("Failed to open the /mkw-sp directory");
            continue;
        }
        while (auto info = dir->read()) {
            if (info->type != NodeType::Dir) {
                continue;
            }
            if (wcslen(info->name) > std::size(m_prefixes[0]) - 1) {
                continue;
            }
            if (wcsncmp(info->name, L"My Stuff", wcslen(L"My Stuff"))) {
                continue;
            }
            swprintf(m_prefixes[m_prefixCount++], std::size(m_prefixes[0]), L"%ls", info->name);
            SP_LOG("Added file replacement prefix %ls", info->name);
        }
        if (m_prefixCount == 0) {
            createDir(L"/mkw-sp/My Stuff", true);
        }

        SP_LOG("Successfully completed initialization");
        m_ok = true;
        return;
    }

    SP_LOG("Failed to find a suitable device");
}

bool FATStorage::ok() const {
    return m_ok;
}

std::optional<FileHandle> FATStorage::fastOpen(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (f_fastopen(file, &m_fs, id) != FR_OK) {
        return {};
    }

    file->m_isOpen = true;
    return file;
}

std::optional<FileHandle> FATStorage::open(const wchar_t *path, const char *mode) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    u32 fMode;
    if (!strcmp(mode, "r")) {
        fMode = FA_READ;
    } else if (!strcmp(mode, "w")) {
        fMode = FA_CREATE_ALWAYS | FA_WRITE;
    } else if (!strcmp(mode, "wx")) {
        fMode = FA_CREATE_NEW | FA_WRITE;
    } else {
        panic("Unknown opening mode");
    }
    auto nodePath = convertPath(path);
    if (!nodePath) {
        return {};
    }
    if (f_open(file, nodePath->path, fMode) != FR_OK) {
        return {};
    }

    file->m_isOpen = true;
    return file;
}

bool FATStorage::createDir(const wchar_t *path, bool allowNop) {
    ScopeLock<Mutex> lock(m_mutex);

    auto nodePath = convertPath(path);
    assert(nodePath);
    FRESULT fResult = f_mkdir(nodePath->path);
    return fResult == FR_OK || (allowNop && fResult == FR_EXIST);
}

std::optional<DirHandle> FATStorage::fastOpenDir(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    if (f_fastopendir(dir, &m_fs, id) != FR_OK) {
        return {};
    }

    dir->m_isOpen = true;
    return dir;
}

std::optional<DirHandle> FATStorage::openDir(const wchar_t *path) {
    ScopeLock<Mutex> lock(m_mutex);

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    auto nodePath = convertPath(path);
    if (!nodePath) {
        return {};
    }
    if (f_opendir(dir, nodePath->path) != FR_OK) {
        return {};
    }

    dir->m_isOpen = true;
    return dir;
}

std::optional<NodeInfo> FATStorage::stat(const wchar_t *path) {
    ScopeLock<Mutex> lock(m_mutex);

    auto nodePath = convertPath(path);
    if (!nodePath) {
        return {};
    }
    FILINFO fInfo;
    if (f_stat(nodePath->path, &fInfo) != FR_OK) {
        return {};
    }

    NodeInfo info{};
    info.id.storage = this;
    info.id.id = fInfo.dir_ofs;
    if (fInfo.fattrib & AM_DIR) {
        info.type = NodeType::Dir;
    } else {
        info.type = NodeType::File;
    }
    info.tick = ConvertTimeToTicks(fInfo.fdate, fInfo.ftime);
    info.size = fInfo.fsize;
    static_assert(sizeof(fInfo.fname) <= sizeof(info.name));
    memcpy(info.name, fInfo.fname, sizeof(fInfo.fname));
    return info;
}

bool FATStorage::rename(const wchar_t *srcPath, const wchar_t *dstPath) {
    ScopeLock<Mutex> lock(m_mutex);

    auto srcNodePath = convertPath(srcPath);
    auto dstNodePath = convertPath(dstPath);
    assert(srcNodePath);
    assert(dstNodePath);
    FRESULT fResult = f_rename(srcNodePath->path, dstNodePath->path);
    return fResult == FR_OK;
}

bool FATStorage::remove(const wchar_t *path, bool allowNop) {
    ScopeLock<Mutex> lock(m_mutex);

    auto nodePath = convertPath(path);
    assert(nodePath);
    FRESULT fResult = f_unlink(nodePath->path);
    return fResult == FR_OK || (allowNop && fResult == FR_NO_FILE);
}

std::optional<FileHandle> FATStorage::startBenchmark() {
    ScopeLock<Mutex> lock(m_mutex);

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (f_open(file, L"benchmark.bin", FA_CREATE_ALWAYS | FA_WRITE | FA_READ) != FR_OK) {
        return {};
    }

    file->m_isOpen = true;
    return file;
}

void FATStorage::endBenchmark() {
    ScopeLock<Mutex> lock(m_mutex);

    f_unlink(L"benchmark.bin");
}

u32 FATStorage::getMessageId() {
    return s_storage->getMessageId();
}

std::optional<FileHandle> FATStorage::File::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    auto *file = FindNode(m_storage->m_files);
    if (file == std::end(m_storage->m_files)) {
        return {};
    }

    *file = *this;
    f_rewind(file);
    return file;
}

bool FATStorage::File::close() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (f_close(this) != FR_OK) {
        return false;
    }

    m_isOpen = false;
    return true;
}

bool FATStorage::File::read(void *dst, u32 size, u32 offset) {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (f_lseek(this, offset) != FR_OK) {
        return false;
    }

    UINT readSize;
    if (f_read(this, dst, size, &readSize) != FR_OK) {
        return false;
    }

    return readSize == size;
}

bool FATStorage::File::write(const void *src, u32 size, u32 offset) {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (f_lseek(this, offset) != FR_OK) {
        return false;
    }

    UINT writtenSize;
    if (f_write(this, src, size, &writtenSize) != FR_OK) {
        return false;
    }

    return writtenSize == size;
}

bool FATStorage::File::sync() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    return f_sync(this) == FR_OK;
}

u64 FATStorage::File::size() {
    return f_size(this);
}

std::optional<DirHandle> FATStorage::Dir::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    auto *dir = FindNode(m_storage->m_dirs);
    if (dir == std::end(m_storage->m_dirs)) {
        return {};
    }

    *dir = *this;
    f_rewinddir(dir);
    return dir;
}

bool FATStorage::Dir::close() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (f_closedir(this) != FR_OK) {
        return false;
    }

    m_isOpen = false;
    return true;
}

std::optional<NodeInfo> FATStorage::Dir::read() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    FILINFO fInfo;
    if (f_readdir(this, &fInfo) != FR_OK) {
        return {};
    }

    if (fInfo.fname[0] == L'\0') {
        return {};
    }

    NodeInfo info{};
    info.id.storage = m_storage;
    info.id.id = fInfo.dir_ofs;
    if (fInfo.fattrib & AM_DIR) {
        info.type = NodeType::Dir;
    } else {
        info.type = NodeType::File;
    }
    info.tick = ConvertTimeToTicks(fInfo.fdate, fInfo.ftime);
    info.size = fInfo.fsize;
    static_assert(sizeof(fInfo.fname) <= sizeof(info.name));
    memcpy(info.name, fInfo.fname, sizeof(fInfo.fname));
    return info;
}

std::optional<FATStorage::Path> FATStorage::convertPath(const wchar_t *path) {
    Path nodePath;

    if (!wcsncmp(path, L"/mkw-sp/", wcslen(L"/mkw-sp/"))) {
        swprintf(nodePath.path, std::size(nodePath.path), L"%ls", path + wcslen(L"/mkw-sp/"));
        return nodePath;
    }

    if (wcsncmp(path, L"ro:/", wcslen(L"ro:/"))) {
        swprintf(nodePath.path, std::size(nodePath.path), L"%ls", path);
        return nodePath;
    }

    if (!wcscmp(path, L"ro:/rel/StaticR.rel")) {
        return {};
    }

    SP::GlobalSettings::FileReplacement fileReplacement =
            SP::GlobalSettings::Get<SP::GlobalSettings::Setting::FileReplacement>();
    switch (fileReplacement) {
    case SP::GlobalSettings::FileReplacement::Off:
        return {};

    case SP::GlobalSettings::FileReplacement::BRSTMsOnly:
        if (!SP::FileReplacement::IsBRSTMFile(path)) {
            return {};
        }
        break;

    case SP::GlobalSettings::FileReplacement::All:
        break;
    }

    for (u32 i = m_prefixCount; i-- > 0;) {
        FILINFO fInfo;
        swprintf(nodePath.path, std::size(nodePath.path), L"%ls/%ls", m_prefixes[i],
                path + wcslen(L"ro:/"));
        if (f_stat(nodePath.path, &fInfo) == FR_OK) {
            return nodePath;
        }
        const wchar_t *bare = path;
        for (const wchar_t *s = path; *s != '\0'; s++) {
            if (*s == L'/') {
                bare = s + 1;
            }
        }
        if (bare == path) {
            continue;
        }
        swprintf(nodePath.path, std::size(nodePath.path), L"%ls/%ls", m_prefixes[i], bare);
        if (f_stat(nodePath.path, &fInfo) == FR_OK) {
            return nodePath;
        }
    }

    return {};
}

const ::FATStorage *FATStorage::Storage() {
    return s_storage;
}

const ::FATStorage *FATStorage::s_storage = nullptr;

} // namespace SP::Storage

extern "C" {
u32 FATStorage_diskSectorSize(void) {
    return SP::Storage::FATStorage::Storage()->diskSectorSize();
}

bool FATStorage_diskRead(u32 firstSector, u32 sectorCount, void *buffer) {
    return SP::Storage::FATStorage::Storage()->diskRead(firstSector, sectorCount, buffer);
}

bool FATStorage_diskWrite(u32 firstSector, u32 sectorCount, const void *buffer) {
    return SP::Storage::FATStorage::Storage()->diskWrite(firstSector, sectorCount, buffer);
}

bool FATStorage_diskErase(u32 firstSector, u32 sectorCount) {
    return SP::Storage::FATStorage::Storage()->diskErase(firstSector, sectorCount);
}

bool FATStorage_diskSync(void) {
    return SP::Storage::FATStorage::Storage()->diskSync();
}
}
