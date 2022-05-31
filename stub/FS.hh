#pragma once

#include <common/IOS.hh>

#include <optional>

namespace IOS {

class FS final : private Resource {
public:
    FS();
    ~FS() = default;

    bool createDir(const char *path, u8 attrs = 0, Mode ownerPerms = Mode::Both,
            Mode groupPerms = Mode::Both, Mode otherPerms = Mode::Both);
    bool rename(const char *srcPath, const char *dstPath);
    bool erase(const char *path);
    bool createFile(const char *path, u8 attrs = 0, Mode ownerPerms = Mode::Both,
            Mode groupPerms = Mode::Both, Mode otherPerms = Mode::Both);

    std::optional<u32> readFile(const char *path, void *dst, u32 size);
    bool writeFile(const char *path, const void *src, u32 size);

private:
    static const char s_path[];
};

} // namespace IOS
