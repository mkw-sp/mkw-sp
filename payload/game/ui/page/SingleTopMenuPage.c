#include <Common.h>

extern void SingleTopMenuPage_ct;
extern void SingleTopMenuPage_onActivate;
extern void SingleTopMenuPage_vf_64;

PATCH_S16(SingleTopMenuPage_ct, 0x2a, 3);
PATCH_S16(SingleTopMenuPage_onActivate, 0x15e, 3);
PATCH_S16(SingleTopMenuPage_vf_64, 0x386, 3);
