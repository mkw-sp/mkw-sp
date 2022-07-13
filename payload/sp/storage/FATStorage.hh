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

    virtual std::optional<FileHandle> fastOpen(u64 id);
    virtual std::optional<FileHandle> open(const wchar_t *path, const char *mode);

    virtual bool createDir(const wchar_t *path, bool allowNop);
    virtual std::optional<DirHandle> fastOpenDir(u64 id);
    virtual std::optional<DirHandle> openDir(const wchar_t *path);

    virtual std::optional<NodeInfo> stat(const wchar_t *path);
    virtual bool rename(const wchar_t *srcPath, const wchar_t *dstPath);
    virtual bool remove(const wchar_t *path, bool allowNop);

    static const ::FATStorage *Storage();

private:
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
        return std::find_if(std::begin(nodes), std::end(nodes), [] (const auto &node) {
            return !node.m_isOpen;
        });
    }

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
