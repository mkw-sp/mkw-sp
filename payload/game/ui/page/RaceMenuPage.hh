#pragma once

#include "game/ui/Button.hh"
#include "game/ui/Page.hh"

namespace UI {

class RaceMenuPage : public Page {
public:
    enum class ButtonId {
        Continue1 = 0x0,
        Quit1 = 0x1,
        Restart1 = 0x2,
        Restart2 = 0x3,
        Replay = 0x4,
        ChangeCourse = 0x5,
        ChangeCharacter = 0x6,
        Next = 0x7,
        Ranking = 0x8,
        ContinueReplay = 0x9,
        RestartReplay = 0xA,
        QuitReplay = 0xB,
        Continue2 = 0xC,
        Quit2 = 0xD,
        BattleGhost = 0xE,
        Restart3 = 0xF,
        Continue3 = 0x10,
        Quit3 = 0x11,
        ChangeMission = 0x12,
        Send1 = 0x13,
        NoSend1 = 0x14,
        GoRanking = 0x15,
        NotGoRanking = 0x16,
        ConfirmContinue = 0x17,
        ConfirmQuit = 0x18,
        SendRecord = 0x19,
        Send2 = 0x1A,
        NoSend2 = 0x1B,
        FriendGhostBattle = 0x1C,
        GoFriendRoom = 0x1D,
        NotGoFriendRoom = 0x1E,
        NextGhost = 0x1F,
        Yes1 = 0x20,
        No1 = 0x21,
        Quit4 = 0x22,
        Yes2 = 0x23,
        No2 = 0x24,
        Settings = 0x25,        // Added
        ChangeGhostData = 0x26, // Added
        SaveState = 0x27,       // Added
        LoadState = 0x28,       // Added
    };

    virtual void setReplacement(PageId id);
    virtual s32 getWindowMessage();
    virtual s32 getButtonCount();
    virtual const ButtonId *getButtons();
    virtual bool getOptionClosable();
    virtual const char *getResFileName();

    static bool REPLACED(IsLastMatch)();
    REPLACE static bool IsLastMatch();
    static TypeInfo *GetTypeInfo();

private:
    void REPLACED(onButtonFront)(PushButton *button, u32 localPlayerId);
    REPLACE void onButtonFront(PushButton *button, u32 localPlayerId);
    void onNextButtonFront(PushButton *button, u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onChangeGhostDataButtonFront(PushButton *button, u32 localPlayerId);

    u8 _044[0x344 - 0x044];
};
static_assert(sizeof(RaceMenuPage) == 0x344);

} // namespace UI
