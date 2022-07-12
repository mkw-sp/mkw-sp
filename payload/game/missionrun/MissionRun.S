#include "Common.S"


PATCH_B_START(SingleTopMenuPage_ct, 0x28)

// Read activator value
lis r12, activateMissionMode@ha
lwz r12, activateMissionMode@l(r12)

li r0, 3

cmpwi r12, 1
bne end_ct

li r0, 4

end_ct:

b SingleTopMenuPage_ct+0x2c
PATCH_B_END(SingleTopMenuPage_ct, 0x28)

PATCH_BL_START(SingleTopMenuPage_vf_84, 0xC0)

// Read activator value
lis r12, activateMissionMode@ha
lwz r12, activateMissionMode@l(r12)

cmpwi r12, 1
bne .foo

lis r5, brctrMissionMode@ha
lwz r5, brctrMissionMode@l (r5)

.foo:

b PushButton_load

PATCH_BL_END(SingleTopMenuPage_vf_84, 0xC0)

PATCH_BL_START(LayoutUIControl_setMessageAll, 0x8)
loc_0x0:
  cmpwi r4, 0xBBC
  beq- loc_0x20
  cmpwi r4, 0xBF0
  beq- loc_0x20
  cmpwi r4, 0xE74
  blt+ loc_0x30
  cmpwi r4, 0xF9F
  bgt+ loc_0x30

loc_0x20:
  stw r4, 28(r1)
  mr r5, r1
  addi r5, r5, 0x1C
  li r4, 0xD7B

loc_0x30:
  lis r6, 0x808C
  
  b LayoutUIControl_setMessageAll+0xc

PATCH_BL_END(LayoutUIControl_setMessageAll, 0x8)