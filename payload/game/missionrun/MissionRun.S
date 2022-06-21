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


/*PATCH_B_START(SingleTopMenuPage_onButtonFront, 0x214)
lis r10, _9d0@ha
lwz r9, _9d0@l(r10)
cmpwi r9, 0x1
bne- end
li r9, 0x0
stw r9, _9d0@l(r10)
li r4, 0x4

end:
cmpwi r4, 0x1

PATCH_B_END(SingleTopMenuPage_onButtonFront, 0x214)*/

// 6th C2 code in the mission mode code, it was better to just port it in asm
PATCH_B_START(SomeMissionModeThing1, 0x1c)
// Read activator value
lis r12, activateMissionMode@h
lwz r12, activateMissionMode@l(r12)

cmpwi r12, 1
bne end_thrd

lis r30, 0x8000
cmplw r4, r30
bgt+ loc_0x10
lis r4, 0x8170

loc_0x10:
  mr r30, r3

end_thrd:
b SomeMissionModeThing1+0x20
PATCH_B_END(SomeMissionModeThing1, 0x1c)

// This tbh could just be ported to C but I've been doing this for 13 fucking hours straight so this is just gonna be asm for now
PATCH_B_START(SomeMissionModeThing2, 0x74)
loc_0x0:
  lis r4, 0x8000
  lwz r4, 2496(r4)
  cmpwi r4, 0x0
  beq+ loc_0x1C
  mr r3, r4
  li r0, 0x0
  b end

loc_0x1C:
  cmpwi r3, 0x0
  beq- loc_0x2C
  lwz r3, 0(r3)
  b end

loc_0x2C:
  lis r3, 0x8170
  li r4, 0x1
  stb r4, 22(r3)
  li r0, 0x0

end:
b SomeMissionModeThing2+0x78
PATCH_B_END(SomeMissionModeThing2, 0x74)