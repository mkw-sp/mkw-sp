#pragma once

#include "sp/ScopeLock.hh"
#include "sp/storage/Storage.hh"

#include <algorithm>
#include <iterator>

namespace SP::Storage {

class DVDStorage : public IStorage {
public:
    DVDStorage();

    std::optional<FileHandle> fastOpen(u64 id) override;
    std::optional<FileHandle> open(const wchar_t *path, const char *mode) override;

    bool createDir(const wchar_t *path, bool allowNop) override;
    std::optional<DirHandle> fastOpenDir(u64 id) override;
    std::optional<DirHandle> openDir(const wchar_t *path) override;

    std::optional<NodeInfo> stat(const wchar_t *path) override;
    bool rename(const wchar_t *srcPath, const wchar_t *dstPath) override;
    bool remove(const wchar_t *path, bool allowNop) override;

    std::optional<FileHandle> startBenchmark() override;
    void endBenchmark() override;
    u32 getMessageId() override;

private:
    class File : public IFile, private DVDFileInfo {
    public:
        std::optional<FileHandle> clone() override;
        bool close() override;
        bool read(void *dst, u32 size, u32 offset) override;
        bool write(const void *src, u32 size, u32 offset) override;
        bool sync() override;
        u64 size() override;

    private:
        DVDStorage *m_storage = nullptr;
        bool m_isOpen = false;

        friend class DVDStorage;
    };

    class Dir : public IDir, private DVDDir {
    public:
        std::optional<DirHandle> clone() override;
        bool close() override;
        std::optional<NodeInfo> read() override;

    private:
        DVDStorage *m_storage = nullptr;
        bool m_isOpen = false;

        friend class DVDStorage;
    };

    struct Path {
        char path[128];
    };
    static std::optional<Path> ConvertPath(const wchar_t *path);

    template <typename N>
    static N *FindNode(N (&nodes)[32]) {
        return std::find_if(std::begin(nodes), std::end(nodes),
                [](const auto &node) { return !node.m_isOpen; });
    }

    Mutex m_mutex{};
    File m_files[32];
    Dir m_dirs[32];
};

} // namespace SP::Storage
