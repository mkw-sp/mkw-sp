#include "DvdArchive.h"

#include <revolution.h>
#include <sp/storage/DecompLoader.h>

#include <stdio.h>

void DvdArchive_mount(DvdArchive *self, EGG_Heap *archiveHeap);

void DvdArchive_move(DvdArchive *self);

static void my_DvdArchive_load(DvdArchive *self, const char *path, EGG_Heap *archiveHeap,
        bool isCompressed, s8 align, EGG_Heap *fileHeap, u32 UNUSED(unused)) {
    if (!isCompressed || !fileHeap) {
        fileHeap = archiveHeap;
    }

    if (self->state == DVD_ARCHIVE_STATE_CLEARED) {
        if (isCompressed) {
            u8 *archiveBuffer;
            size_t archiveSize;
            if (DecompLoader_LoadRO(path, &archiveBuffer, &archiveSize, archiveHeap)) {
                self->archiveBuffer = archiveBuffer;
                self->archiveSize = archiveSize;
                self->archiveHeap = archiveHeap;
                self->state = DVD_ARCHIVE_STATE_DECOMPRESSED;
                DvdArchive_mount(self, archiveHeap);
            }
        } else {
            DvdArchive_rip(self, path, fileHeap, align);
            if (self->state == DVD_ARCHIVE_STATE_RIPPED) {
                DvdArchive_move(self);
                DvdArchive_mount(self, archiveHeap);
            }
        }
    }
}
PATCH_B(DvdArchive_load, my_DvdArchive_load);

void DvdArchive_loadOther(DvdArchive *self, DvdArchive *other, EGG_Heap *heap);

static void my_DvdArchive_loadOther(DvdArchive *self, DvdArchive *other, EGG_Heap *UNUSED(heap)) {
    if (self->state != DVD_ARCHIVE_STATE_CLEARED || other->state != DVD_ARCHIVE_STATE_MOUNTED) {
        return;
    }

    self->archiveBuffer = other->archiveBuffer;
    self->archiveSize = other->archiveSize;
    self->archiveHeap = other->archiveHeap;
    self->state = DVD_ARCHIVE_STATE_DECOMPRESSED;
    DvdArchive_mount(self, self->archiveHeap);
    other->archiveBuffer = NULL;
    other->archiveSize = 0;
    other->archiveHeap = NULL;
    DvdArchive_clear(other);
}
PATCH_B(DvdArchive_loadOther, my_DvdArchive_loadOther);
