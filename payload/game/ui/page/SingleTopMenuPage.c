#include <Common.h>

#include <game/ui/SectionManager.h>
#include <game/ui/Section.h>
#include <game/ui/page/TopMenuPage.h>

#include <game/ui/page/SingleTopMenuPage.h>


//PATCH_S16(SingleTopMenuPage_ct, 0x2a, 3);
PATCH_S16(SingleTopMenuPage_onActivate, 0x15e, 3);
PATCH_S16(SingleTopMenuPage_vf_64, 0x386, 3);
