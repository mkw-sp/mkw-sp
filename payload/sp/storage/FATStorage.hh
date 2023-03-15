#pragma once

#include "sp/ScopeLock.hh"
extern "C" {
#include "sp/storage/FATStorage.h"
}
#include "sp/storage/Storage.hh"

#include <algorithm>
#include <iterator>

extern "C" {
#include <ff/ff.h>
}

namespace SP::Storage {

class FATStorage : public IStorage {
public:
    FATStorage();
    bool ok() const;

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

    static const ::FATStorage *Storage();

private:
    static OSTime convertTimeToTicks(NodeInfo info);

    class File : public IFile, private FIL {
    public:
        std::optional<FileHandle> clone() override;
        bool close() override;
        bool read(void *dst, u32 size, u32 offset) override;
        bool write(const void *src, u32 size, u32 offset) override;
        bool sync() override;
        u64 size() override;

    private:
        FATStorage *m_storage = nullptr;
        bool m_isOpen = false;

        friend class FATStorage;
    };

    class Dir : public IDir, private DIR {
    public:
        std::optional<DirHandle> clone() override;
        bool close() override;
        std::optional<NodeInfo> read() override;

    private:
        FATStorage *m_storage = nullptr;
        bool m_isOpen = false;

        friend class FATStorage;
    };

    struct Path {
        wchar_t path[128];
    };
    std::optional<Path> convertPath(const wchar_t *path);

    template <typename N>
    static N *FindNode(N (&nodes)[32]) {
        return std::find_if(std::begin(nodes), std::end(nodes),
                [](const auto &node) { return !node.m_isOpen; });
    }

    static OSTime ConvertTimeToTicks(u16 date, u16 time);

    Mutex m_mutex{};
    FATFS m_fs;
    File m_files[32];
    Dir m_dirs[32];
    u32 m_prefixCount = 0;
    wchar_t m_prefixes[32][32];
    bool m_ok = false;

    static const ::FATStorage *s_storage;
};

} // namespace SP::Storage
