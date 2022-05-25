#pragma once

#include <common/IOS.hh>

namespace IOS {

class FS final : private Resource {
public:
    FS();
    ~FS() = default;

    bool createFile(const char *path, u8 attrs, Mode ownerPerms,
            Mode groupPerms, Mode otherPerms);
    bool rename(const char *srcPath, const char *dstPath);

    bool writeFile(const char *path, const void *src, u32 size);

private:
    static const char s_path[];
};

} // namespace IOS
