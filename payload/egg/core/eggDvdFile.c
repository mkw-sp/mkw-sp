#include "eggDvdFile.h"

#include <nw4r/ut/ut_list.h>

extern ut_List s_dvdList;

static bool my_EGG_DvdFile_openPath(EGG_DvdFile *self, const char *path) {
    if (self->isOpen) {
        return true;
    }

    self->isOpen = DVDOpen(path, &self->fileInfo);
    if (!self->isOpen) {
        return false;
    }

    ut_List_Append(&s_dvdList, self);
    return true;
}
PATCH_B(EGG_DvdFile_openPath, my_EGG_DvdFile_openPath);
