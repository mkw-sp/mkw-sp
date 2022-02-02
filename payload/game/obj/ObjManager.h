#include "ObjGroup.h"

typedef struct {
    u8 _000[0x058 - 0x000];
    ObjGroup *group;
    u8 _05c[0x160 - 0x05c];
} ObjManager;
static_assert(sizeof(ObjManager) == 0x160);

void ObjManager_groupInsert(ObjManager *this, Obj *obj);

u32 ObjManager_groupGetCount(const ObjManager *this);
