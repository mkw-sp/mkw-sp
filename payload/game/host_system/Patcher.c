#include "Patcher.h"

#include <revolution.h>

#include <string.h>

#include <Dol.h>
#include <Rel.h>

extern const Patch __start_patches;
extern const Patch __stop_patches;

static u32 getBinary(void *address) {
    if (address >= Dol_getStart() && address < Dol_getEnd()) {
        return PATCHER_BINARY_DOL;
    }

    if (address >= Rel_getStart() && address < Rel_getEnd()) {
        return PATCHER_BINARY_REL;
    }

    return PATCHER_BINARY_NONE;
}

void Patcher_patch(u32 binary) {
    for (const Patch *patch = &__start_patches; patch < &__stop_patches; patch++) {
        void *dst;
        void *src;
        u32 size;

        u32 branch_inst;

        switch (patch->type) {
        case PATCH_TYPE_WRITE:
            dst = patch->write.dst;
            src = patch->write.src;
            size = patch->write.size;
            break;
        case PATCH_TYPE_BRANCH:
            branch_inst = 0x12 << 26 | ((patch->branch.to - patch->branch.from) & 0x3fffffc);
            branch_inst |= !!patch->branch.link;

            dst = patch->branch.from;
            src = &branch_inst;
            size = sizeof(u32);
            break;
        default:
            continue;
        }

        if (getBinary(dst) != binary) {
            continue;
        }

        memcpy(dst, src, size);
        DCFlushRange(dst, size);
        ICInvalidateRange(dst, size);
    }
}
