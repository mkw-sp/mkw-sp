#include "FS.hh"

#include <algorithm>
#include <cstring>

namespace IOS {

namespace Ioctl {
    enum {
        CreateDir = 0x3,
        Delete = 0x7,
        Rename = 0x8,
        CreateFile = 0x9,
        GetFileStats = 0xb,
    };
} // namespace Ioctl

FS::FS() : Resource(ALIGNED_STRING("/dev/fs"), Mode::None) {}

FS::~FS() = default;

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

bool FS::erase(const char *path) {
    alignas(0x20) char in[0x40];

    if (strlcpy(in, path, 0x40) >= 0x40) {
        return false;
    }

    return ioctl(Ioctl::Delete, in, sizeof(in), nullptr, 0) == 0;
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

s32 File::getStats(Stats *stats) {
    alignas(0x20) Stats out;

    s32 result = ioctl(Ioctl::GetFileStats, nullptr, 0, &out, sizeof(out));
    if (result >= 0) {
        *stats = out;
    }
    return result;
}

std::optional<u32> FS::readFile(const char *path, void *dst, u32 size) {
    File file(path, Mode::Read);
    if (!file.ok()) {
        return false;
    }

    File::Stats stats;
    if (file.getStats(&stats) < 0 || stats.size > size) {
        return false;
    }

    s32 result = file.read(dst, size);
    if (result < 0) {
        return {};
    }
    return result;
}

bool FS::writeFile(const char *path, const void *src, u32 size) {
    erase(path);
    createFile(path);

    File file(path, Mode::Write);
    if (!file.ok()) {
        return false;
    }

    s32 result = file.write(src, size);
    return result >= 0 && static_cast<u32>(result) == size;
}

bool FS::copyFile(const char *srcPath, const char *dstPath) {
    erase(dstPath);
    createFile(dstPath);

    File dstFile(dstPath, Mode::Write);
    if (!dstFile.ok()) {
        return false;
    }

    File srcFile(srcPath, Mode::Read);
    if (!srcFile.ok()) {
        return false;
    }

    File::Stats stats;
    if (srcFile.getStats(&stats) < 0) {
        return false;
    }

    u32 size = stats.size;
    while (size > 0) {
        alignas(0x20) u8 chunk[8192];
        u32 chunkSize = std::min(static_cast<size_t>(size), sizeof(chunk));
        s32 result = srcFile.read(chunk, chunkSize);
        if (result < 0 || static_cast<u32>(result) != chunkSize) {
            return false;
        }
        result = dstFile.write(chunk, chunkSize);
        if (result < 0 || static_cast<u32>(result) != chunkSize) {
            return false;
        }
        size -= chunkSize;
    }
    return true;
}

} // namespace IOS
