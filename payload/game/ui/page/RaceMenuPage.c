#include "RaceMenuPage.h"

#include "game/system/RaceConfig.h"
#include "game/ui/SectionManager.h"
#include "game/ui/ctrl/Button.h"

// Note: Could do these as an X-Macro

typedef enum {
    kPausePageButtonID_Continue1 = 0x0,
    kPausePageButtonID_Quit1 = 0x1,
    kPausePageButtonID_Restart1 = 0x2,
    kPausePageButtonID_Restart2 = 0x3,
    kPausePageButtonID_Replay = 0x4,
    kPausePageButtonID_ChangeCourse = 0x5,
    kPausePageButtonID_ChangeCharacter = 0x6,
    kPausePageButtonID_Next = 0x7,
    kPausePageButtonID_Ranking = 0x8,
    kPausePageButtonID_ContinueReplay = 0x9,
    kPausePageButtonID_RestartReplay = 0xA,
    kPausePageButtonID_QuitReplay = 0xB,
    kPausePageButtonID_Continue2 = 0xC,
    kPausePageButtonID_Quit2 = 0xD,
    kPausePageButtonID_BattleGhost = 0xE,
    kPausePageButtonID_Restart3 = 0xF,
    kPausePageButtonID_Continue3 = 0x10,
    kPausePageButtonID_Quit3 = 0x11,
    kPausePageButtonID_ChangeMission = 0x12,
    kPausePageButtonID_Send1 = 0x13,
    kPausePageButtonID_NoSend1 = 0x14,
    kPausePageButtonID_GoRanking = 0x15,
    kPausePageButtonID_NotGoRanking = 0x16,
    kPausePageButtonID_ConfirmContinue = 0x17,
    kPausePageButtonID_ConfirmQuit = 0x18,
    kPausePageButtonID_SendRecord = 0x19,
    kPausePageButtonID_Send2 = 0x1A,
    kPausePageButtonID_NoSend2 = 0x1B,
    kPausePageButtonID_FriendGhostBattle = 0x1C,
    kPausePageButtonID_GoFriendRoom = 0x1D,
    kPausePageButtonID_NotGoFriendRoom = 0x1E,
    kPausePageButtonID_NextGhost = 0x1F,
    kPausePageButtonID_Yes1 = 0x20,
    kPausePageButtonID_No1 = 0x21,
    kPausePageButtonID_Quit4 = 0x22,
    kPausePageButtonID_Yes2 = 0x23,
    kPausePageButtonID_No2 = 0x24,

    // Reserved: Edit license settings in-game
    kPausePageButtonID_EXTLicenseSettings = 0x25,
    kPausePageButtonID_EXTChangeGhostData = 0x26,
    kPausePageButtonID_EXTSaveState = 0x27,
    kPausePageButtonID_EXTLoadState = 0x28,
} PausePageButtonID;

// Referenced by RaceMenuPage.S
const char *sButtonStrings[] = {
        "ButtonContinue",          // kPausePageButtonID_Continue1
        "ButtonQuit",              // kPausePageButtonID_Quit1
        "ButtonRestart",           // kPausePageButtonID_Restart1
        "ButtonRestart",           // kPausePageButtonID_Restart2
        "ButtonReplay",            // kPausePageButtonID_Replay
        "ButtonChangeCourse",      // kPausePageButtonID_ChangeCourse
        "ButtonChangeCharacter",   // kPausePageButtonID_ChangeCharacter
        "ButtonNext",              // kPausePageButtonID_Next
        "ButtonRanking",           // kPausePageButtonID_Ranking
        "ButtonContinueReplay",    // kPausePageButtonID_ContinueReplay
        "ButtonRestartReplay",     // kPausePageButtonID_RestartReplay
        "ButtonQuitReplay",        // kPausePageButtonID_QuitReplay
        "ButtonContinue",          // kPausePageButtonID_Continue2
        "ButtonQuit",              // kPausePageButtonID_Quit2
        "ButtonBattleGhost",       // kPausePageButtonID_BattleGhost
        "ButtonRestart",           // kPausePageButtonID_Restart3
        "ButtonContinue",          // kPausePageButtonID_Continue3
        "ButtonQuit",              // kPausePageButtonID_Quit3
        "ButtonChangeMission",     // kPausePageButtonID_ChangeMission
        "ButtonSend",              // kPausePageButtonID_Send1
        "ButtonNoSend",            // kPausePageButtonID_NoSend1
        "ButtonGoRanking",         // kPausePageButtonID_GoRanking
        "ButtonNotGoRanking",      // kPausePageButtonID_NotGoRanking
        "ButtonConfirmContinue",   // kPausePageButtonID_ConfirmContinue
        "ButtonConfirmQuit",       // kPausePageButtonID_ConfirmQuit
        "ButtonSendRecord",        // kPausePageButtonID_SendRecord
        "ButtonSend",              // kPausePageButtonID_Send2
        "ButtonNoSend",            // kPausePageButtonID_NoSend2
        "ButtonFriendGhostBattle", // kButtonID_FriendGhostBattle
        "ButtonGoFriendRoom",      // kPausePageButtonID_GoFriendRoom
        "ButtonNotGoFriendRoom",   // kPausePageButtonID_NotGoFriendRoom
        "ButtonNextGhost",         // kPausePageButtonID_NextGhost
        "ButtonYes",               // kPausePageButtonID_Yes1
        "ButtonNo",                // kPausePageButtonID_No1
        "ButtonQuit",              // kPausePageButtonID_Quit4
        "ButtonYes",               // kPausePageButtonID_Yes2
        "ButtonNo",                // kPausePageButtonID_No2

        "ButtonSettings",        // kPausePageButtonID_EXTLicenseSettings
        "ButtonChangeGhostData", // kPausePageButtonID_EXTChangeGhostData
        "ButtonSaveState",       // kPausePageButtonID_EXTSaveState
        "ButtonLoadState",       // kPausePageButtonID_EXTLoadState
};

static const u32 ghostWatchPauseButtons[] = {
        kPausePageButtonID_Continue2,
        kPausePageButtonID_Restart3,
        kPausePageButtonID_BattleGhost,
        kPausePageButtonID_EXTChangeGhostData,
        kPausePageButtonID_ChangeCourse,
        kPausePageButtonID_ChangeCharacter,
        kPausePageButtonID_EXTLicenseSettings,
        kPausePageButtonID_Quit1,
};

u32 GhostWatchPauseMenuPage_getButtonCount(void);

static u32 my_GhostWatchPauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(ghostWatchPauseButtons);
}
PATCH_B(GhostWatchPauseMenuPage_getButtonCount, my_GhostWatchPauseMenuPage_getButtonCount);

const u32 *GhostWatchPauseMenuPage_getButtons(void);

static const u32 *my_GhostWatchPauseMenuPage_getButtons(void) {
    return ghostWatchPauseButtons;
}
PATCH_B(GhostWatchPauseMenuPage_getButtons, my_GhostWatchPauseMenuPage_getButtons);

bool GhostWatchPauseMenuPage_vf_74(void);

static bool my_GhostWatchPauseMenuPage_vf_74(void) {
    // Allow closing with option
    return true;
}
PATCH_B(GhostWatchPauseMenuPage_vf_74, my_GhostWatchPauseMenuPage_vf_74);

static const u32 afterTaButtons[] = {
        kPausePageButtonID_Restart2,
        kPausePageButtonID_EXTChangeGhostData,
        kPausePageButtonID_ChangeCourse,
        kPausePageButtonID_ChangeCharacter,
        kPausePageButtonID_Replay,
        kPausePageButtonID_Quit1,
};

u32 AfterTimeAttackMenuPage_getButtonCount(void);

static u32 my_AfterTimeAttackMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(afterTaButtons);
}
PATCH_B(AfterTimeAttackMenuPage_getButtonCount, my_AfterTimeAttackMenuPage_getButtonCount);

const u32 *AfterTimeAttackMenuPage_getButtons(void);

static const u32 *my_AfterTimeAttackMenuPage_getButtons(void) {
    return afterTaButtons;
}
PATCH_B(AfterTimeAttackMenuPage_getButtons, my_AfterTimeAttackMenuPage_getButtons);

static const u32 vsPauseButtons[] = {
        kPausePageButtonID_Continue1,
        kPausePageButtonID_Restart1,
        kPausePageButtonID_EXTLicenseSettings,
        kPausePageButtonID_Quit1,
};

u32 VsPauseMenuPage_getButtonCount(void);

static u32 my_VsPauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(vsPauseButtons);
}
PATCH_B(VsPauseMenuPage_getButtonCount, my_VsPauseMenuPage_getButtonCount);

const u32 *VsPauseMenuPage_getButtons(void);

static const u32 *my_VsPauseMenuPage_getButtons(void) {
    return vsPauseButtons;
}
PATCH_B(VsPauseMenuPage_getButtons, my_VsPauseMenuPage_getButtons);

const char *VsPauseMenuPage_getFile(void);

static const char *my_VsPauseMenuPage_getFile(void) {
    return "PauseMenuVS";
}
PATCH_B(VsPauseMenuPage_getFile, my_VsPauseMenuPage_getFile);

static const u32 battlePauseButtons[] = {
        kPausePageButtonID_Continue1,
        kPausePageButtonID_Restart1,
        kPausePageButtonID_EXTLicenseSettings,
        kPausePageButtonID_Quit1,
};

u32 BattlePauseMenuPage_getButtonCount(void);

static u32 my_BattlePauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(battlePauseButtons);
}
PATCH_B(BattlePauseMenuPage_getButtonCount, my_BattlePauseMenuPage_getButtonCount);

const u32 *BattlePauseMenuPage_getButtons(void);

static const u32 *my_BattlePauseMenuPage_getButtons(void) {
    return battlePauseButtons;
}
PATCH_B(BattlePauseMenuPage_getButtons, my_BattlePauseMenuPage_getButtons);

const char *BattlePauseMenuPage_getFile(void);

static const char *my_BattlePauseMenuPage_getFile(void) {
    return "PauseMenuVS";
}
PATCH_B(BattlePauseMenuPage_getFile, my_BattlePauseMenuPage_getFile);

static const u32 timeAttackPauseButtons[] = {
        kPausePageButtonID_Continue1,
        kPausePageButtonID_Restart1,
        kPausePageButtonID_EXTSaveState,
        kPausePageButtonID_EXTLoadState,
        kPausePageButtonID_EXTChangeGhostData,
        kPausePageButtonID_ChangeCourse,
        kPausePageButtonID_ChangeCharacter,
        kPausePageButtonID_EXTLicenseSettings,
        kPausePageButtonID_Quit1,
};

u32 TimeAttackPauseMenuPage_getButtonCount(void);

static u32 my_TimeAttackPauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(timeAttackPauseButtons);
}
PATCH_B(TimeAttackPauseMenuPage_getButtonCount, my_TimeAttackPauseMenuPage_getButtonCount);

const u32 *TimeAttackPauseMenuPage_getButtons(void);

static const u32 *my_TimeAttackPauseMenuPage_getButtons(void) {
    return timeAttackPauseButtons;
}
PATCH_B(TimeAttackPauseMenuPage_getButtons, my_TimeAttackPauseMenuPage_getButtons);

static const u32 replayTAPauseButtons[] = {
        kPausePageButtonID_ContinueReplay,
        kPausePageButtonID_RestartReplay,
        kPausePageButtonID_EXTLicenseSettings,
        kPausePageButtonID_QuitReplay,
};
u32 ReplayTAPauseMenuPage_getButtonCount(void);
u32 my_ReplayTAPauseMenuPage_getButtonCount(void) {
    return ARRAY_SIZE(replayTAPauseButtons);
}
PATCH_B(ReplayTAPauseMenuPage_getButtonCount, my_ReplayTAPauseMenuPage_getButtonCount);

const u32 *ReplayTAMenuPage_getButtons(void);

static const u32 *my_ReplayTAMenuPage_getButtons(void) {
    return replayTAPauseButtons;
}

PATCH_B(ReplayTAMenuPage_getButtons, my_ReplayTAMenuPage_getButtons);

// Mission mode

// Event buttons
static const u32 EventPauseButtons[] = {
        kPausePageButtonID_Continue1,
        kPausePageButtonID_Restart1,
        kPausePageButtonID_ChangeCharacter,
        kPausePageButtonID_Quit1,
};

// Mission buttons
static const u32 MissionRunPauseButtons[] = {
        kPausePageButtonID_Continue1,
        kPausePageButtonID_Restart1,
        kPausePageButtonID_ChangeMission,
        kPausePageButtonID_Quit1,
};

// Get the right BRCTR depending on the section id
const char *FUN_806331a0(void);
static const char *my_FUN_806331a0(void) {
    if (s_sectionManager->currentSection->id == SECTION_ID_MR_REPLAY) {
        return "PauseMenuMR";
    }
    return "PauseMenuEvent";
}
PATCH_B(FUN_806331a0, my_FUN_806331a0);

// Get the right button array depending on the section id
const u32 *EventPauseMenuPage_getButtons(void);
static const u32 *my_EventPauseMenuPage_getButtons(void) {
    if (s_sectionManager->currentSection->id == SECTION_ID_MR_REPLAY) {
        return MissionRunPauseButtons;
    }
    return EventPauseButtons;
}

PATCH_B(EventPauseMenuPage_getButtons, my_EventPauseMenuPage_getButtons);
