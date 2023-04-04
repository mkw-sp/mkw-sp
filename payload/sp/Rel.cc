#include "Rel.hh"

extern "C" {
#include "sp/Host.h"
#include "sp/Patcher.h"
#include "sp/Payload.h"
#include "sp/security/Memory.h"
#include "sp/security/Stack.h"
}
#include "sp/storage/Storage.hh"

extern "C" {
#include <game/system/Console.h>
#include <revolution.h>
}

#include <cstring>

namespace SP::Rel {

typedef void (*EntryFunction)(void);

struct ModuleInfo {
    u8 hash[NET_SHA1_DIGEST_SIZE];
    u32 size;
};

static EntryFunction entry = nullptr;

static const std::array<ModuleInfo, 4> moduleInfoArray = {
        // clang-format off
    // RMCE
    ModuleInfo
    {
        {
            0x07, 0x2B, 0xA4, 0x43, 0x82, 0xFD, 0xCA, 0x9B, 0xF5, 0xC0,
            0xEC, 0x9D, 0x1E, 0x9B, 0x92, 0x2C, 0xAF, 0xD9, 0x92, 0x60,
        },
        0x004ACF94,
    },
    // RMCP
    ModuleInfo
    {
        {
            0x88, 0x7B, 0xCC, 0x07, 0x67, 0x81, 0xF5, 0xB0, 0x05, 0xCC,
            0x31, 0x7A, 0x6E, 0x3C, 0xC8, 0xFD, 0x5F, 0x91, 0x13, 0x00,
        },
        0x004AD3C4,
    },
    // RMCJ
    ModuleInfo
    {
        {
            0x20, 0xB4, 0x16, 0x1D, 0x41, 0x5E, 0x40, 0x92, 0xA6, 0x29,
            0x9B, 0xB2, 0x85, 0xFA, 0xA6, 0x22, 0x83, 0x5B, 0x2B, 0x16,
        },
        0x004ACABC,
    },
    // RMCK
    ModuleInfo
    {
        {
            0xE0, 0x00, 0x62, 0x50, 0xB1, 0xE5, 0xC1, 0xBD, 0xEE, 0x8B,
            0xDB, 0x0E, 0x78, 0x9E, 0xA2, 0x00, 0xB3, 0xA8, 0x46, 0xE4,
        },
        0x004AD9FC,
    },
        // clang-format on
};

static bool IsClean(const void *rel, u32 roundedRelSize) {
    u32 regionIndex;
    switch (REGION) {
    case REGION_E:
        regionIndex = 0;
        break;
    case REGION_P:
        regionIndex = 1;
        break;
    case REGION_J:
        regionIndex = 2;
        break;
    case REGION_K:
        regionIndex = 3;
        break;
    default:
        panic("Invalid game region!");
    }

    u32 relSize = moduleInfoArray[regionIndex].size;
    if (roundedRelSize < relSize) {
        return false;
    }

    u8 hash[NET_SHA1_DIGEST_SIZE];
    NETSHA1Context context;
    NETSHA1Init(&context);
    NETSHA1Update(&context, rel, relSize);
    NETSHA1GetDigest(&context, hash);

    return memcmp(moduleInfoArray[regionIndex].hash, hash, NET_SHA1_DIGEST_SIZE) == 0;
}

bool Load() {
    auto file = Storage::OpenRO("/rel/StaticR.rel");
    if (!file) {
        return false;
    }

#ifndef GDB_COMPATIBLE
    void *src = OSGetMEM1ArenaLo();

    if (!file->read(src, file->size(), 0)) {
        return false;
    }
    if (!IsClean(src, file->size())) {
        return false;
    }

    void *dst = Rel_getStart();
    auto *srcHeader = reinterpret_cast<OSModuleHeader *>(src);
    memcpy(dst, src, srcHeader->fixSize);
    ICInvalidateRange(dst, srcHeader->fixSize);
    auto *dstHeader = reinterpret_cast<OSModuleHeader *>(dst);

    void *bss = reinterpret_cast<void *>(
            AlignUp(reinterpret_cast<size_t>(dst) + srcHeader->fixSize, 0x20));
    memset(bss, 0, srcHeader->bssSize);

    dstHeader->info.sectionInfoOffset += reinterpret_cast<u32>(dst);
    auto *dstSectionInfo = reinterpret_cast<OSSectionInfo *>(dstHeader->info.sectionInfoOffset);
    for (u32 i = 1; i < dstHeader->info.numSections; i++) {
        if (dstSectionInfo[i].offset != 0) {
            dstSectionInfo[i].offset += reinterpret_cast<u32>(dst);
        } else if (dstSectionInfo[i].size != 0) {
            dstSectionInfo[i].offset = reinterpret_cast<u32>(bss);
        }
    }

    dstHeader->impOffset += reinterpret_cast<u32>(src);
    auto *importInfo = reinterpret_cast<OSImportInfo *>(dstHeader->impOffset);
    for (u32 i = 0; i < dstHeader->impSize / sizeof(OSImportInfo); i++) {
        importInfo[i].offset += reinterpret_cast<u32>(src);
    }

    Relocate(NULL, dstHeader);
    Relocate(dstHeader, dstHeader);

    OSSectionInfo *prologSectionInfo = dstSectionInfo + dstHeader->prologSection;
    entry = reinterpret_cast<EntryFunction>(prologSectionInfo->offset + dstHeader->prolog);
    return true;
#else
    if (!file->read(Rel_getStart(), file->size(), 0)) {
        return false;
    }
    if (!IsClean(Rel_getStart(), file->size())) {
        return false;
    }

    auto *dst = reinterpret_cast<OSModuleHeader *>(Rel_getStart());
    auto *bss = reinterpret_cast<void *>(0x809BD6E0);

    OSLink(dst, bss);
    entry = reinterpret_cast<EntryFunction>(dst->prolog);
    return true;
#endif
}

void Run() {
    assert(entry);
#ifndef GDB_COMPATIBLE
    Stack_DoLinkRegisterPatches(reinterpret_cast<u32 *>(Rel_getTextSectionStart()),
            reinterpret_cast<u32 *>(Rel_getTextSectionEnd()));
#endif
    Patcher_patch(PATCHER_BINARY_REL);
    Memory_ProtectRange(OS_PROTECT_CHANNEL_2, Rel_getTextSectionStart(), Rel_getRodataSectionEnd(),
            OS_PROTECT_PERMISSION_READ);
    Memory_ProtectRange(OS_PROTECT_CHANNEL_3, Payload_getTextSectionStart(),
            Payload_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);

    if (HostPlatform_IsDolphin(Host_GetPlatform())) {
        Console_init();
    }
    entry();
}

} // namespace SP::Rel

extern "C" void Rel_Run() {
    SP::Rel::Run();
}
