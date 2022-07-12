#include <game/ui/SectionManager.h>
#include <game/ui/page/SingleTopMenuPage.h>
#include <game/ui/Section.h>
#include <game/ui/page/TopMenuPage.h>

#include <game/missionrun/MissionRun.h>

int activateMissionMode = 1;
const char* brctrMissionMode = "SingleTopMR";

int my_RaceRankingPage_getReplacement() {
    if (s_sectionManager->currentSection->id == SECTION_ID_MR_REPLAY)
        return PAGE_ID_AFTER_MR_MENU;
    return PAGE_ID_AFTER_TOURNAMENT_MENU;
}

const char* my_getResultCtrlName() {
    int sectionId = s_sectionManager->currentSection->id;
    if (sectionId == SECTION_ID_GP || sectionId == 0x70 || sectionId == 0x71)
        return "ResultGP";
    if (sectionId == SECTION_ID_TIME_ATTACK || sectionId == SECTION_ID_GHOST_TA || sectionId == SECTION_ID_GHOST_TA_ONLINE || sectionId == SECTION_ID_MR_REPLAY || sectionId == SECTION_ID_TOURNAMENT_REPLAY)
        return "ResultTA";
    return "ResultVS";
}

PATCH_B(RaceRankingPage_getReplacement, my_RaceRankingPage_getReplacement);
PATCH_B(getResultCtrlName, my_getResultCtrlName);