#include <Common.S>

// Copy singular/plural handling from the Korean version

PATCH_REPLACE_START(TextBox_setMessage, 0xde0)
    cmpwi r3, 0x4
    blt 0xc
    cmpwi r3, 0x8
    ble 0x20
    cmpwi r3, 0x0
    blt 0xc
    cmpwi r3, 0x2
PATCH_REPLACE_END(TextBox_setMessage, 0xde0)



PATCH_REPLACE_START(Font_805cf154, 0x88)
    andi. r0, r0, 0x1
PATCH_REPLACE_END(Font_805cf154, 0x88)

PATCH_BL_START(GlyphRenderer_setMaterial, 0xf8)
    li r3, 0x2 // Original instruction (K)

    // Check that the region is not K
    lis r4, 0x8000
    lhz r4, 0x620a (r4)
    cmpwi r4, 0x5511
    beqlr

    li r4, 0x0
    li r5, 0x0
    bl GXSetTevSwapMode

    lwz r9, 0x0 (r30) // Original instruction (non-K)
    b GlyphRenderer_setMaterial + 0xfc
PATCH_BL_END(GlyphRenderer_setMaterial, 0xf8)

PATCH_BL_START(GlyphRenderer_setMaterial, 0x150)
    add r0, r0, r5 // Original instruction (K)

    // Check that the region is not K
    lis r6, 0x8000
    lhz r6, 0x620a (r6)
    cmpwi r6, 0x5511
    beqlr

    bl GXSetTevColor // Original instruction (non-K)

    li r3, 0x0
    bl GXSetTevDirect

    li r3, 0x1
    bl GXSetTevDirect

    li r3, 0x2
    bl GXSetTevDirect

    li r3, 0x0
    li r4, 0x0
    li r5, 0x0
    li r6, 0xff
    bl GXSetTevOrder

    li r3, 0x1
    li r4, 0xff
    li r5, 0xff
    li r6, 0xff
    bl GXSetTevOrder

    li r3, 0x2
    li r4, 0xff
    li r5, 0xff
    li r6, 0xff
    bl GXSetTevOrder

    b GlyphRenderer_setMaterial + 0x22c
PATCH_BL_END(GlyphRenderer_setMaterial, 0x150)

PATCH_REPLACE_START(Font_805d00b0, 0x110)
    andi. r0, r0, 0x1
PATCH_REPLACE_END(Font_805d00b0, 0x110)

PATCH_BL_START(Font_buildTexObjs, 0x30)
    lhz r0, 0xa (r29) // Original instruction

    cmpwi r0, 0x0
    bnelr

    li r3, 0x2
    stw r3, 0x20 (r27)

    b Font_buildTexObjs + 0x34
PATCH_BL_END(Font_buildTexObjs, 0x30)
