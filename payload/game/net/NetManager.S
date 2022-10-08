#include <Common.S>

PATCH_REPLACE_START(NetManager_createInstance, 0x0)
    blr
PATCH_REPLACE_END(NetManager_createInstance, 0x0)

// We already initialize in RootScene
//
// NOTE: We already allocated a networking heap in RootScene
// This means NetScene gets 650kb for just DWC, since it doesn't handle SO anymore.
// Since most of that usage is for DWC itself, this is quite small.
//
PATCH_REPLACE_START(NetManager_connect, 0x38)
    nop // +0 [bl SOInit]
    li r3, 0 // +4 [bl SOStartup]
PATCH_REPLACE_END(NetManager_connect, 0x38)

PATCH_REPLACE_START(NetManager_806561a8, 0x0)
    blr
PATCH_REPLACE_END(NetManager_806561a8, 0x0)

PATCH_REPLACE_START(NetManager_80656a30, 0x0)
    blr
PATCH_REPLACE_END(NetManager_80656a30, 0x0)

PATCH_REPLACE_START(NetManager_80656d9c, 0x0)
    blr
PATCH_REPLACE_END(NetManager_80656d9c, 0x0)

PATCH_REPLACE_START(NetManager_80656e44, 0x0)
    li r3, 1
    blr
PATCH_REPLACE_END(NetManager_80656e44, 0x0)
