#include <game/ui/SectionManager.h>
#include <game/ui/page/SingleTopMenuPage.h>
#include <game/ui/Section.h>
#include <game/ui/page/TopMenuPage.h>

#include <game/missionrun/MissionRun.h>

int activateMissionMode = 1;
const char* brctrMissionMode = "SingleTopMR";

void(*backup_668)(MenuPage*, PushButton*);
u32 randomPlaceWith0;

u32 _9c0;
u32 _9d0;

void missionModeFunc1() {
    Section* currentSection = s_sectionManager->currentSection;
    TopMenuPage* topMenuPage = (TopMenuPage*)currentSection->pages[PAGE_ID_SINGLE_TOP_MENU];

    topMenuPage->_6b4 = -1;
    topMenuPage->_6b8 = missionModeFunc2;
    backup_668 = topMenuPage->_668;

    randomPlaceWith0 = 0;

    // So for some reason loading these pages crashes when gdb_compatible is on
    Section_addPages(currentSection, 148);
}

void missionModeFunc2() {
    MenuPage* menuPage = (MenuPage*)s_sectionManager->currentSection->pages[PAGE_ID_SINGLE_TOP_MENU];

    _9d0 = 1;

    backup_668(menuPage, (PushButton*)menuPage->controlGroup.parent);
}