#pragma once

#include <optional>

#include <Common.hh>

namespace SP::Storage {

class IStorage;

struct NodeId {
    IStorage *storage;
    u64 id;
};

enum class NodeType {
    File,
    Dir,
};

struct NodeInfo {
    NodeId id;
    NodeType type;
    u64 size;
    wchar_t name[255 + 1];
};

class FileHandle;

class IFile {
public:
    virtual std::optional<FileHandle> clone() = 0;
    virtual bool close() = 0;
    virtual bool read(void *dst, u32 size, u32 offset) = 0;
    virtual bool write(const void *src, u32 size, u32 offset) = 0;
    virtual bool sync() = 0;
    virtual u64 size() = 0;
};

class DirHandle;

class IDir {
public:
    virtual std::optional<DirHandle> clone() = 0;
    virtual bool close() = 0;
    virtual std::optional<NodeInfo> read() = 0;
};

class FileHandle {
public:
    FileHandle(IFile *file);
    FileHandle(const FileHandle &) = delete;
    FileHandle(FileHandle &&);
    FileHandle &operator=(FileHandle &&);
    ~FileHandle();

    std::optional<FileHandle> clone();
    bool read(void *dst, u32 size, u32 offset);
    bool write(const void *src, u32 size, u32 offset);
    bool sync();
    u64 size();

private:
    IFile *m_file;
};

class DirHandle {
public:
    std::optional<DirHandle> clone();
    DirHandle(IDir *dir);
    DirHandle(const DirHandle &) = delete;
    DirHandle(DirHandle &&);
    DirHandle &operator=(DirHandle &&);
    ~DirHandle();

    std::optional<NodeInfo> read();

private:
    IDir *m_dir;
};

class IStorage {
public:
    virtual std::optional<FileHandle> fastOpen(u64 id) = 0;
    virtual std::optional<FileHandle> open(const wchar_t *path, const char *mode) = 0;

    virtual bool createDir(const wchar_t *path, bool allowNop) = 0;
    virtual std::optional<DirHandle> fastOpenDir(u64 id) = 0;
    virtual std::optional<DirHandle> openDir(const wchar_t *path) = 0;

    virtual std::optional<NodeInfo> stat(const wchar_t *path) = 0;
    virtual bool rename(const wchar_t *srcPath, const wchar_t *dstPath) = 0;
    virtual bool remove(const wchar_t *path, bool allowNop) = 0;

    virtual std::optional<FileHandle> startBenchmark() = 0;
    virtual void endBenchmark() = 0;
    virtual u32 getMessageId() = 0;
};

enum class StorageType {
    Net,
    FAT,
    NANDArchive,
    DVD,
};

struct Throughputs {
    u32 sizes[4];
    u32 read[4];
    u32 write[4];
};

struct BenchmarkStatus {
    enum class Mode {
        Read,
        Write,
    };

    u32 size;
    Mode mode;
};

bool Init();

std::optional<FileHandle> FastOpen(NodeId id);
std::optional<FileHandle> Open(const wchar_t *path, const char *mode);
std::optional<FileHandle> OpenRO(const char *path);
std::optional<u32> FastReadFile(NodeId id, void *dst, u32 size);
std::optional<u32> ReadFile(const wchar_t *path, void *dst, u32 size);
bool WriteFile(const wchar_t *path, const void *src, u32 size, bool overwrite);

bool CreateDir(const wchar_t *path, bool allowNop);
std::optional<DirHandle> FastOpenDir(NodeId id);
std::optional<DirHandle> OpenDir(const wchar_t *path);

std::optional<NodeInfo> Stat(const wchar_t *path);
bool Rename(const wchar_t *srcPath, const wchar_t *dstPath);
bool Remove(const wchar_t *path, bool allowNop);

static constexpr u32 BENCHMARK_BUFFER_SIZE = 1024 * 1024;
std::optional<Throughputs> Benchmark(StorageType type, void *buffer);
std::optional<BenchmarkStatus> GetBenchmarkStatus();
u32 GetMessageId(StorageType type);

} // namespace SP::Storage
