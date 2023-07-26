#pragma once

#include "IOS.hh"

namespace IOS {

class FS final : private Resource {
public:
    FS();
    ~FS();
    using Resource::ok;

    bool createDir(const char *path, u8 attrs = 0, Mode ownerPerms = Mode::Both,
            Mode groupPerms = Mode::Both, Mode otherPerms = Mode::Both);
    bool rename(const char *srcPath, const char *dstPath);
    bool erase(const char *path);
    bool createFile(const char *path, u8 attrs = 0, Mode ownerPerms = Mode::Both,
            Mode groupPerms = Mode::Both, Mode otherPerms = Mode::Both);

    std::optional<u32> readFile(const char *path, void *dst, u32 size);
    bool writeFile(const char *path, const void *src, u32 size);
    bool copyFile(const char *srcPath, const char *dstPath);
};

} // namespace IOS
