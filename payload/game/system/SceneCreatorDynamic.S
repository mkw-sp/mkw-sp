#include <Common.S>

DoCreateOther:
    b SceneCreatorDynamic_createOther

PATCH_B_START(SceneCreatorDynamic_create, 0xb0)
    cmpwi r3, 0
    bne SceneAlreadyMade
        # r3 = thisptr
        # r4 = u32 id
        bl SceneCreatorDynamic_createOther
    SceneAlreadyMade:
    b SceneCreatorDynamic_create + 0xb4
PATCH_B_END(SceneCreatorDynamic_create, 0xb0)

#if 0
// Forces a load into scene SCENE_ID_SPFATAL
PATCH_B_START(SceneCreatorDynamic_create, 0)
    lis r12, sBugCheckSet@ha
    lwz r12, sBugCheckSet@l(r12)
    cmpwi r12, 0
    beq BugCheckNotSet
        // sBugCheckSet != 0
        li r4, 100
        b DoCreateOther
    BugCheckNotSet:
        stwu r1, -16(r1) # Original instruction
        b SceneCreatorDynamic_create + 4
    PATCH_B_END(SceneCreatorDynamic_create, 0)
#endif

PATCH_B_START(lyt_TextBox_CT, 0x200)
    // Looking up 'font' fails on our custom archive for some reason.
    // This is slightly slower but makes our code work.
    li r4, 0
    b lyt_TextBox_CT + 0x204
PATCH_B_END(lyt_TextBox_CT, 0x200)

NoFont:
    mr r4, r31
    bl endStr
        .string "TextBox: Failed to load font %s\n\0"
        .align 4
    endStr:
    mflr r3
    bl OSReport
    b lyt_TextBox_CT + 0x26c

PATCH_B_START(lyt_TextBox_CT, 0x218)
    // 8007B538
    beq NoFont
    b lyt_TextBox_CT + 0x21C
PATCH_B_END(lyt_TextBox_CT, 0x218)
