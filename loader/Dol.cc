#include "Dol.hh"

#include <cstring>

extern "C" {
#include <vendor/sha1/sha1.h>
}

namespace Dol {

struct ModuleInfo {
    u8 hash[20];
    u32 size;
};

static const std::array<ModuleInfo, 4> moduleInfoArray = {
        // clang-format off
        // RMCP
        ModuleInfo
        {
            {
                0x3C, 0x0E, 0xB6, 0x78, 0xBC, 0x11, 0x86, 0x93, 0x71, 0xE7,
                0x2D, 0x46, 0xA6, 0xBB, 0xC3, 0xA1, 0x69, 0x5A, 0x48, 0x48,
            },
            0x00385140,
        },
        // RMCE
        ModuleInfo
        {
            {
                0x28, 0xDA, 0xFA, 0x4C, 0xD2, 0x8A, 0xB7, 0x1C, 0xC8, 0xC4,
                0x4E, 0x64, 0xC8, 0x4B, 0x27, 0x66, 0x5B, 0x68, 0x36, 0x97,
            },
            0x00380DC0,
        },
        // RMCJ
        ModuleInfo
        {
            {
                0x3E, 0x4F, 0x90, 0x52, 0xCA, 0xF1, 0xCB, 0xF0, 0xC8, 0x71,
                0xA3, 0x10, 0x60, 0xE1, 0x17, 0x67, 0xFE, 0x09, 0x31, 0xAE,
            },
            0x00384AC0,
        },
        // RMCK
        ModuleInfo
        {
            {
                0xFF, 0x41, 0xE4, 0x22, 0x20, 0x0C, 0x2F, 0x37, 0x94, 0x29,
                0xC3, 0x48, 0x99, 0x3D, 0x21, 0xEB, 0xE7, 0x6E, 0x61, 0xE2,
            },
            0x00373160,
        },
        // clang-format on
};

static std::optional<u32> GetRegionIndex() {
    switch (*reinterpret_cast<char *>(0x80000003)) {
    case 'P':
        return 0;
    case 'E':
        return 1;
    case 'J':
        return 2;
    case 'K':
        return 3;
    default:
        return {};
    }
}

const void *GetStartAddress() {
    return reinterpret_cast<const void *>(0x80004000);
}

std::optional<u32> GetSize() {
    std::optional regionIndex = GetRegionIndex();
    if (!regionIndex.has_value()) {
        return {};
    }

    return moduleInfoArray[*regionIndex].size;
}

std::optional<bool> IsClean() {
    std::optional regionIndex = GetRegionIndex();
    if (!regionIndex.has_value()) {
        return {};
    }

    char hash[20];
    SHA1(hash, reinterpret_cast<const char *>(GetStartAddress()),
            moduleInfoArray[*regionIndex].size);

    return memcmp(moduleInfoArray[*regionIndex].hash, hash, 20) == 0;
}

} // namespace Dol
