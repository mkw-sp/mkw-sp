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