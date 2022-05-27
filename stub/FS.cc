#include "FS.hh"

#include <cstring>

namespace IOS {

namespace Ioctl {
    enum {
        CreateDir = 0x3,
        Rename = 0x8,
        CreateFile = 0x9,
    };
} // namespace Ioctl

const char FS::s_path[] = "/dev/fs";

FS::FS() : Resource(s_path, Mode::None) {}

bool FS::createDir(const char *path, u8 attrs, Mode ownerPerms, Mode groupPerms, Mode otherPerms) {
    alignas(0x20) u8 in[0x4c];

    if (strlcpy(reinterpret_cast<char *>(in + 0x6), path, 0x40) >= 0x40) {
        return false;
    }
    in[0x46] = static_cast<u8>(ownerPerms);
    in[0x47] = static_cast<u8>(groupPerms);
    in[0x48] = static_cast<u8>(otherPerms);
    in[0x49] = attrs;

    return ioctl(Ioctl::CreateDir, in, sizeof(in), nullptr, 0) == 0;
}

bool FS::rename(const char *srcPath, const char *dstPath) {
    alignas(0x20) char in[0x80];

    if (strlcpy(in + 0x0, srcPath, 0x40) >= 0x40) {
        return false;
    }
    if (strlcpy(in + 0x40, dstPath, 0x40) >= 0x40) {
        return false;
    }

    return ioctl(Ioctl::Rename, in, sizeof(in), nullptr, 0) == 0;
}

bool FS::createFile(const char *path, u8 attrs, Mode ownerPerms, Mode groupPerms, Mode otherPerms) {
    alignas(0x20) u8 in[0x4c];

    if (strlcpy(reinterpret_cast<char *>(in + 0x6), path, 0x40) >= 0x40) {
        return false;
    }
    in[0x46] = static_cast<u8>(ownerPerms);
    in[0x47] = static_cast<u8>(groupPerms);
    in[0x48] = static_cast<u8>(otherPerms);
    in[0x49] = attrs;

    return ioctl(Ioctl::CreateFile, in, sizeof(in), nullptr, 0) == 0;
}

bool FS::writeFile(const char *path, const void *src, u32 size) {
    createFile(path, 0, Mode::Both, Mode::Both, Mode::Both);

    File file(path, Mode::Write);
    if (!file.ok()) {
        return false;
    }
    s32 result = file.write(src, size);
    return result >= 0 && static_cast<u32>(result) == size;
}

} // namespace IOS
