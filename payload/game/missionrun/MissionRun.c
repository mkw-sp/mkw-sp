#include <game/ui/SectionManager.h>
#include <game/ui/page/SingleTopMenuPage.h>
#include <game/ui/Section.h>
#include <game/ui/page/TopMenuPage.h>

#include <game/missionrun/MissionRun.h>

int activateMissionMode = 1;
const char* brctrMissionMode = "SingleTopMR";

void(*backup_668)(MenuPage*, PushButton*);

u32 _9c0;
u32 _9d0;

void missionModeFunc1() {
    Section* currentSection = s_sectionManager->currentSection;
    TopMenuPage* topMenuPage = (TopMenuPage*)currentSection->pages[PAGE_ID_SINGLE_TOP_MENU];

    topMenuPage->_6b4 = -1;
    topMenuPage->_6b8 = missionModeFunc2;
    backup_668 = topMenuPage->_668;

    Section_addPages(currentSection, 148);
}

void missionModeFunc2() {
    MenuPage* menuPage = (MenuPage*)s_sectionManager->currentSection->pages[PAGE_ID_SINGLE_TOP_MENU];

    _9d0 = 1;

    backup_668(menuPage, (PushButton*)menuPage->controlGroup.parent);
}

const char* my_getResultCtrlName() {
    int sectionId = s_sectionManager->currentSection->id;
    if (sectionId == SECTION_ID_GP || sectionId == 0x70 || sectionId == 0x71)
        return "ResultGP";
    if (sectionId == SECTION_ID_TIME_ATTACK || sectionId == SECTION_ID_GHOST_TA || sectionId == SECTION_ID_GHOST_TA_ONLINE || sectionId == SECTION_ID_MR_REPLAY || sectionId == SECTION_ID_TOURNAMENT_REPLAY)
        return "ResultTA";
    return "ResultVS";
}

PATCH_B(getResultCtrlName, my_getResultCtrlName);