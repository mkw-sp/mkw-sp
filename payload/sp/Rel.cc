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

static EntryFunction entry = nullptr;

extern "C" void *REPLACED(memset_slow)(void *s, int c, size_t n);

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

    void *dst = Rel_getStart();
    auto *srcHeader = reinterpret_cast<OSModuleHeader *>(src);
    memcpy(dst, src, srcHeader->fixSize);
    ICInvalidateRange(dst, srcHeader->fixSize);
    auto *dstHeader = reinterpret_cast<OSModuleHeader *>(dst);

    void *bss = reinterpret_cast<u8 *>(dst) + OSRoundUp32B(srcHeader->fixSize);
    REPLACED(memset_slow)(bss, 0, srcHeader->bssSize);

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

    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_3, Rel_getTextSectionStart(), Rel_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_0, Payload_getTextSectionStart(), Payload_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    
    if (HostPlatform_IsDolphin(Host_GetPlatform())) {
        Console_init();
    }
    entry();
}

} // namespace SP::Rel

extern "C" void Rel_Run() {
    SP::Rel::Run();
}
