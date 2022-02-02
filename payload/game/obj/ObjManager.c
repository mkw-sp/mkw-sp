#include "ObjManager.h"

ObjManager *ObjManager_ct(ObjManager *this);

PATCH_S16(ObjManager_ct, 0x1a2, sizeof(ObjGroup));

static void my_ObjManager_groupInsert(ObjManager *this, Obj *obj) {
    this->group->objs[this->group->count++] = obj;
}
PATCH_B(ObjManager_groupInsert, my_ObjManager_groupInsert);

static u32 my_ObjManager_groupGetCount(const ObjManager *this) {
    if (!this->group) {
        return 0;
    }

    return this->group->count;
}
PATCH_B(ObjManager_groupGetCount, my_ObjManager_groupGetCount);
