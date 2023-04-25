#include "Storage.hh"

#include "sp/storage/DVDStorage.hh"
#include "sp/storage/FATStorage.hh"
#include "sp/storage/NANDArchiveStorage.hh"
#include "sp/storage/NetStorage.hh"

#include <common/Bytes.hh>

namespace SP::Storage {

static std::optional<NetStorage> netStorage{};
static std::optional<FATStorage> fatStorage{};
static std::optional<NANDArchiveStorage> nandArchiveStorage{};
static DVDStorage dvdStorage;
static std::optional<BenchmarkStatus> benchmarkStatus{};

FileHandle::FileHandle(IFile *file) : m_file(file) {}

FileHandle::FileHandle(FileHandle &&that) : m_file(that.m_file) {
    that.m_file = nullptr;
}

FileHandle &FileHandle::operator=(FileHandle &&that) {
    return that;
}

FileHandle::~FileHandle() {
    if (m_file) {
        m_file->close();
    }
}

std::optional<FileHandle> FileHandle::clone() {
    return m_file->clone();
}

bool FileHandle::read(void *dst, u32 size, u32 offset) {
    assert(dst);

    return m_file->read(dst, size, offset);
}

bool FileHandle::write(const void *src, u32 size, u32 offset) {
    assert(src);

    return m_file->write(src, size, offset);
}

bool FileHandle::sync() {
    return m_file->sync();
}

u64 FileHandle::size() {
    return m_file->size();
}

DirHandle::DirHandle(IDir *dir) : m_dir(dir) {}

DirHandle::DirHandle(DirHandle &&that) : m_dir(that.m_dir) {
    that.m_dir = nullptr;
}

DirHandle &DirHandle::operator=(DirHandle &&that) {
    return that;
}

DirHandle::~DirHandle() {
    if (m_dir) {
        m_dir->close();
    }
}

std::optional<DirHandle> DirHandle::clone() {
    return m_dir->clone();
}

std::optional<NodeInfo> DirHandle::read() {
    return m_dir->read();
}

bool Init() {
    if (!netStorage) {
        netStorage.emplace();
    }
    if (!fatStorage) {
        fatStorage.emplace();
    }
    if (!fatStorage->ok()) {
        fatStorage.reset();
        return false;
    }
    if (!nandArchiveStorage) {
        nandArchiveStorage.emplace();
    }
    if (!nandArchiveStorage->ok()) {
        nandArchiveStorage.reset();
        return false;
    }

    return true;
}

IStorage *GetStorage(StorageType type) {
    switch (type) {
    case StorageType::Net:
        return &*netStorage;
    case StorageType::FAT:
        return &*fatStorage;
    case StorageType::NANDArchive:
        return &*nandArchiveStorage;
    case StorageType::DVD:
        return &dvdStorage;
    default:
        return nullptr;
    }
}

template <typename R, typename... Args>
R Dispatch(R (IStorage::*function)(Args... args), Args... args) {
    {
        R result = ((*netStorage).*function)(args...);
        if (result) {
            return result;
        }
    }

    {
        R result = ((*fatStorage).*function)(args...);
        if (result) {
            return result;
        }
    }

    {
        R result = ((*nandArchiveStorage).*function)(args...);
        if (result) {
            return result;
        }
    }

    return (dvdStorage.*function)(args...);
}

std::optional<FileHandle> FastOpen(NodeId id) {
    assert(id.storage);

    return id.storage->fastOpen(id.id);
}

std::optional<FileHandle> Open(const wchar_t *path, const char *mode) {
    assert(path);
    assert(mode);

    return Dispatch(&IStorage::open, path, mode);
}

std::optional<FileHandle> OpenRO(const char *path) {
    assert(path);

    wchar_t roPath[128];
    if (path[0] == '/') {
        swprintf(roPath, std::size(roPath), L"ro:%s", path);
    } else {
        swprintf(roPath, std::size(roPath), L"ro:/%s", path);
    }
    return Open(roPath, "r");
}

std::optional<u32> FastReadFile(NodeId id, void *dst, u32 size) {
    auto file = FastOpen(id);
    if (!file) {
        return {};
    }

    size = std::min(static_cast<u64>(size), file->size());
    if (!file->read(dst, size, 0)) {
        return {};
    }
    return size;
}

std::optional<u32> ReadFile(const wchar_t *path, void *dst, u32 size) {
    auto file = Open(path, "r");
    if (!file) {
        return {};
    }

    size = std::min(static_cast<u64>(size), file->size());
    if (!file->read(dst, size, 0)) {
        return {};
    }
    return size;
}

bool WriteFile(const wchar_t *path, const void *src, u32 size, bool overwrite) {
    if (overwrite) {
        wchar_t newPath[256];
        swprintf(newPath, std::size(newPath), L"%ls.new", path);
        wchar_t oldPath[256];
        swprintf(oldPath, std::size(oldPath), L"%ls.old", path);

        {
            auto file = Open(newPath, "w");
            if (!file) {
                return false;
            }
            if (!file->write(src, size, 0)) {
                SP_LOG("Failed to write to %ls", newPath);
                return false;
            }
        }

        if (!Remove(oldPath, true)) {
            SP_LOG("Failed to remove %ls", oldPath);
            return false;
        }

        if (!Rename(path, oldPath)) {
            SP_LOG("Failed to rename %ls to %ls", path, oldPath);
            // Ignore
        }

        if (!Rename(newPath, path)) {
            SP_LOG("Failed to rename %ls to %ls", newPath, path);
            return false;
        }

        Remove(oldPath, true); // Not a big deal if this fails
        return true;
    } else {
        auto file = Open(path, "wx");
        if (!file) {
            return false;
        }
        return file->write(src, size, 0);
    }
}

bool CreateDir(const wchar_t *path, bool allowNop) {
    assert(path);

    return Dispatch(&IStorage::createDir, path, allowNop);
}

std::optional<DirHandle> FastOpenDir(NodeId id) {
    assert(id.storage);

    return id.storage->fastOpenDir(id.id);
}

std::optional<DirHandle> OpenDir(const wchar_t *path) {
    assert(path);

    return Dispatch(&IStorage::openDir, path);
}

std::optional<DirHandle> OpenRODir(const char *path) {
    assert(path);

    wchar_t roPath[128];
    if (path[0] == '/') {
        swprintf(roPath, std::size(roPath), L"ro:%s", path);
    } else {
        swprintf(roPath, std::size(roPath), L"ro:/%s", path);
    }
    return OpenDir(roPath);
}

std::optional<NodeInfo> Stat(const wchar_t *path) {
    assert(path);

    return Dispatch(&IStorage::stat, path);
}

bool Rename(const wchar_t *srcPath, const wchar_t *dstPath) {
    assert(srcPath);
    assert(dstPath);

    return Dispatch(&IStorage::rename, srcPath, dstPath);
}

bool Remove(const wchar_t *path, bool allowNop) {
    assert(path);

    return Dispatch(&IStorage::remove, path, allowNop);
}

static std::optional<Throughputs> Benchmark(FileHandle file, void *buffer) {
    for (u32 i = 0; i < 8; i++) {
        u8 seed[hydro_random_SEEDBYTES] = {};
        Bytes::Write<u32>(seed, 0, i);
        memset(seed, i, sizeof(seed));
        hydro_random_buf_deterministic(buffer, BENCHMARK_BUFFER_SIZE, seed);
        if (!file.write(buffer, BENCHMARK_BUFFER_SIZE, i * BENCHMARK_BUFFER_SIZE)) {
            return {};
        }
    }

    {
        u8 seed[hydro_random_SEEDBYTES] = {};
        hydro_random_buf_deterministic(buffer, BENCHMARK_BUFFER_SIZE, seed);
    }

    Throughputs throughputs;
    u32 sizes[] = {1024 * 1024, 128 * 1024, 16 * 1024, 2 * 1024};
    for (u32 i = 0; i < std::size(sizes); i++) {
        throughputs.sizes[i] = sizes[i];
        u32 count = 8 * BENCHMARK_BUFFER_SIZE / sizes[i];

        {
            {
                ScopeLock<NoInterrupts> lock;
                benchmarkStatus = {sizes[i], BenchmarkStatus::Mode::Read};
            }
            OSTime startTime = OSGetTime();
            for (u32 j = 0; j < count; j++) {
                u8 seed[hydro_random_SEEDBYTES] = {};
                Bytes::Write<u32>(seed, 0, i);
                Bytes::Write<u32>(seed, 4, j);

                u32 k;
                hydro_random_buf_deterministic(&k, sizeof(k), seed);
                k %= count;

                if (!file.read(buffer, sizes[i], k * sizes[i])) {
                    ScopeLock<NoInterrupts> lock;
                    benchmarkStatus.reset();
                    return {};
                }
            }
            OSTime duration = OSGetTime() - startTime;
            throughputs.read[i] = OSSecondsToTicks(UINT64_C(8) * BENCHMARK_BUFFER_SIZE) / duration;
        }

        {
            {
                ScopeLock<NoInterrupts> lock;
                benchmarkStatus = {sizes[i], BenchmarkStatus::Mode::Write};
            }
            OSTime startTime = OSGetTime();
            for (u32 j = 0; j < count; j++) {
                u8 seed[hydro_random_SEEDBYTES] = {};
                Bytes::Write<u32>(seed, 0, i);
                Bytes::Write<u32>(seed, 4, j);

                u32 k;
                hydro_random_buf_deterministic(&k, sizeof(k), seed);
                k %= count;

                if (!file.write(buffer, sizes[i], k * sizes[i])) {
                    ScopeLock<NoInterrupts> lock;
                    benchmarkStatus.reset();
                    return {};
                }
            }
            OSTime duration = OSGetTime() - startTime;
            throughputs.write[i] = OSSecondsToTicks(UINT64_C(8) * BENCHMARK_BUFFER_SIZE) / duration;
        }
    }

    ScopeLock<NoInterrupts> lock;
    benchmarkStatus.reset();
    return throughputs;
}

std::optional<Throughputs> Benchmark(StorageType type, void *buffer) {
    auto *storage = GetStorage(type);
    if (!storage) {
        return {};
    }
    auto file = storage->startBenchmark();
    if (!file) {
        return {};
    }

    auto result = Benchmark(std::move(*file), buffer);

    storage->endBenchmark();

    return result;
}

std::optional<BenchmarkStatus> GetBenchmarkStatus() {
    return benchmarkStatus;
}

u32 GetMessageId(StorageType type) {
    auto *storage = GetStorage(type);
    return storage ? storage->getMessageId() : 0;
}

} // namespace SP::Storage
