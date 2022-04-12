#include "BootStrapScene.h"
#include "Patcher.h"

#include <Rel.h>
#include <sp/Memory.h>
#include <sp/Stack.h>
#include <string.h>


#include <game/system/Console.h>
#include <sp/Host.h>

typedef void (*RelEntryFunction)(void);

static void *loadRel(void *arg) {
    EGG_Heap *heap = arg;

    DVDFileInfo fileInfo;
    if (!DVDOpen("/rel/StaticR.rel", &fileInfo)) {
        return NULL;
    }

    s32 size = OSRoundUp32B(fileInfo.length);
#ifndef GDB_COMPATIBLE
    void *src = spAlloc(size, 0x20, heap);

    s32 result = DVDRead(&fileInfo, src, size, 0);
    DVDClose(&fileInfo);
    if (result != size) {
        return NULL;
    }

    void *dst = Rel_getStart();
    OSModuleHeader *srcHeader = src;
    memcpy(dst, src, srcHeader->fixSize);
    ICInvalidateRange(dst, srcHeader->fixSize);
    OSModuleHeader *dstHeader = dst;

    void *bss = dst + OSRoundUp32B(srcHeader->fixSize);
    memset(bss, 0, srcHeader->bssSize);

    dstHeader->info.sectionInfoOffset += (u32)dst;
    OSSectionInfo *dstSectionInfo = (OSSectionInfo *)dstHeader->info.sectionInfoOffset;
    for (u32 i = 1; i < dstHeader->info.numSections; i++) {
        if (dstSectionInfo[i].offset != 0) {
            dstSectionInfo[i].offset += (u32)dst;
        } else if (dstSectionInfo[i].size != 0) {
            dstSectionInfo[i].offset = (u32)bss;
        }
    }

    dstHeader->impOffset += (u32)src;
    OSImportInfo *importInfo = (OSImportInfo *)dstHeader->impOffset;
    for (u32 i = 0; i < dstHeader->impSize / sizeof(OSImportInfo); i++) {
        importInfo[i].offset += (u32)src;
    }

    Relocate(NULL, dstHeader);
    Relocate(dstHeader, dstHeader);

    spFree(src);

    OSSectionInfo *prologSectionInfo = dstSectionInfo + dstHeader->prologSection;
    return (void *)(prologSectionInfo->offset + dstHeader->prolog);
#else
    s32 result = DVDRead(&fileInfo, Rel_getStart(), size, 0);
    DVDClose(&fileInfo);
    if (result != size) {
        return NULL;
    }

    OSModuleHeader *dst = (OSModuleHeader *)Rel_getStart();
    void *bss = (void *)0x809BD6E0;

    OSLink(dst, bss);
    return (void *)dst->prolog;
#endif
}

void my_BootStrapScene_calc(BootStrapScene *this) {
    RelEntryFunction entry;
    if (!OSJoinThread(this->relLoadThread, (void **)&entry)) {
        return;
    }

    if (!entry) {
        // TODO handle
        return;
    }

    Patcher_patch(PATCHER_BINARY_REL);
#ifndef GDB_COMPATIBLE
    // The game will crash if the following function is ran before the '.rel' module is patched
    Stack_DoLinkRegisterPatches(Rel_getTextSectionStart(), Rel_getTextSectionEnd());
#endif
    Memory_ProtectRangeModule(OS_PROTECT_CHANNEL_3, Rel_getTextSectionStart(), Rel_getRodataSectionEnd(), OS_PROTECT_PERMISSION_READ);
    
    if (HostPlatform_IsDolphin(Host_GetPlatform())) {
        Console_init();
    }
    entry();
}
PATCH_B(BootStrapScene_calc, my_BootStrapScene_calc);

void my_BootStrapScene_draw(BootStrapScene *UNUSED(this)) {}
PATCH_B(BootStrapScene_draw, my_BootStrapScene_draw);

void my_BootStrapScene_enter(BootStrapScene *this) {
    this->relLoadThread = spAlloc(sizeof(OSThread), 0x4, this->heapMem1);
    u32 stackSize = 0x5000;
    this->relLoadThreadStack = spAlloc(stackSize, 0x4, this->heapMem1);
    void *stackBase = this->relLoadThreadStack + stackSize;
    OSCreateThread(this->relLoadThread, loadRel, this->heapMem1, stackBase, stackSize, 20, 0);
    OSResumeThread(this->relLoadThread);
}
PATCH_B(BootStrapScene_enter, my_BootStrapScene_enter);

void my_BootStrapScene_exit(BootStrapScene *this) {
    OSDetachThread(this->relLoadThread);
    spFree(this->relLoadThreadStack);
    spFree(this->relLoadThread);
}
PATCH_B(BootStrapScene_exit, my_BootStrapScene_exit);
