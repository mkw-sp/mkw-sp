#include "game/host_system/SystemManager.h"

#include <sp/storage/DecompLoader.h>

#include <stdio.h>

void *HomeButton_getFile(void *r3, const char *path, EGG_Heap *heap, bool isCompressed,
        u32 *fileSize);

static void *my_HomeButton_getFile(void */* r3 */, const char *path, EGG_Heap *heap,
        bool isCompressed, u32 *fileSize) {
    if (isCompressed) {
        u8 *file;
        size_t tmpSize;
        bool result = DecompLoader_LoadRO(path, &file, &tmpSize, heap);
        assert(result);
        if (fileSize) {
            *fileSize = tmpSize;
        }
        return file;
    } else {
        u8 *file = SystemManager_ripFromDisc(path, heap, false, fileSize);
        assert(file);
        return file;
    }
}
PATCH_B(HomeButton_getFile, my_HomeButton_getFile);
