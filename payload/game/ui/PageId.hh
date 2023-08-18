#pragma once

namespace UI {

enum class PageId {
    None = -0x1,
    Empty = 0x0,
    EsrbNotice = 0x1,
    FpsNotice = 0x2,
    CorruptSave = 0x3,
    UnableToSave = 0x4,
    CorruptNand = 0x5,
    CorruptMiis = 0x6,
    GpPanOverlay = 0x7,
    VsPanOverlay = 0x8,
    BtPanOverlay = 0x9,
    MrPanOverlay = 0xA,
    ToPanOverlay = 0xB,
    GpHud = 0xC,
    TaHud = 0xD,
    Vs1pHud = 0xE,
    Vs2pHud = 0xF,
    Vs3pHud = 0x10,
    Vs4pHud = 0x11,
    Bt1pHud = 0x12,
    Bt2pHud = 0x13,
    Bt3pHud = 0x14,
    Bt4pHud = 0x15,
    MrToHud = 0x16,
    GpPauseMenu = 0x17,
    VsPauseMenu = 0x18,
    TaPauseMenu = 0x19,
    BattlePauseMenu = 0x1A,
    MrPauseMenu = 0x1B,
    TaGhostPauseMenu = 0x1C,
    Unknown1D = 0x1D, // "Abandon Ghost Race?" on wiki, cannot find the activation in game
    Unknown1E = 0x1E, // "Quit Ghost Race?" on wiki, cannot find the activation in game
    GhostReplayPauseMenu = 0x1F,
    AfterGpMenu = 0x20,
    AfterTaMenu = 0x21,
    AfterVsMenu = 0x22,
    AfterBtMenu = 0x23,
    AfterBtFinal = 0x24,
    AfterMrMenu = 0x25,
    AfterToMenu = 0x26,
    AfterGrMenu = 0x27,
    GotoFriendRoster = 0x28,
    SendGhostRecord = 0x29,
    SendTournamentRecord = 0x2A,
    CheckRankings = 0x2B,
    ConfirmQuit = 0x2C,
    AfterTaMenuSplits = 0x2D,
    AfterTaMenuLeaderboard = 0x2E,
    ResultRaceUpdate = 0x2F,
    ResultRaceTotal = 0x30,
    ResultWifiUpdate = 0x31, // Unused
    ResultTeamVSTotal = 0x32,
    ResultBattleUpdate = 0x33,
    ResultBattleTotal = 0x34,
    CompetitionPersonalLeaderboard = 0x35,
    GpReplayHud = 0x36,
    GhostReplayHud = 0x37,
    GpReplayPauseMenu = 0x38,
    TaReplayPauseMenu = 0x39,
    Unknown3A = 0x3A, // "Supporting Panel, present in many modes in-race, 2nd element"
    AwardRunup = 0x3B,
    AwardInterface = 0x3C,
    Unknown3D = 0x3D,
    Unknown3E = 0x3E,
    CreditsCongrats = 0x3F,

    Wifi1pHud = 0x40,
    Wifi2pHud = 0x41,
    WifiFriendHud = 0x42,
    WifiTeamFriendHud = 0x43,
    Unknown44 = 0x44, // Untested: "Congratuations at end of friend room"
    Unknown45 = 0x45, // "Dummy? Seems to immediately load 0x46"
    AfterWifiMenu = 0x46,
    ConfirmWifiQuit = 0x47,
    OnlinePleaseWait = 0x48,
    Unknown49 = 0x49, // "Live VS view interface"
    Unknown4A = 0x4A, // "Live Battle view interface"
    RaceConfirm = 0x4B,
    SpinnerMessagePopup = 0x4C,
    MessagePopup = 0x4D,
    YesNoPopup = 0x4E,
    SpinnerAwait = 0x4F,
    ConnectingNintendoWfc = 0x50,
    MenuMessage = 0x51,
    Confirm = 0x52,
    MessageBoardPopup = 0x53,
    Unknown54 = 0x54, // "Behind main menu?" however isn't activated on main menu.
    LowBatteryPopupManager = 0x55,
    LowBatteryPopup = 0x56,
    Title = 0x57,
    Unknown58 = 0x58, // "Behind main menu?" however isn't activated on main menu.
    OpeningMovie = 0x59,
    TopMenu = 0x5A,
    Unknown5B = 0x5B, // "Behind unlocks?"
    Model = 0x5C,
    LineBackgroundWhite = 0x5D,
    Obi = 0x5E,
    PressA = 0x5F,
    SelectMii = 0x60,
    PlayerPad = 0x61, // Activates ControllerRegister when activated
    ControllerRegister = 0x62,
    ControllerRegisterInstructions = 0x63,
    ControllerRegisterComplete = 0x64,
    LicenseSelect = 0x65,
    LicenceDisplay = 0x66,
    LicenseSettingsTop = 0x67,
    LicenceEraseConfirm = 0x68,
    SingleTop = 0x69,
    GpClassSelect = 0x6A,
    CharacterSelect = 0x6B,
    VehicleSelect = 0x6C,
    DriftSelect = 0x6D,
    CourseSelect = 0x6E, // Replaces RaceCupSelect
    // Disabled {
    RaceCourseSelect = 0x6F,
    // }
    TimeAttackTop = 0x70,
    TimeAttackGhostList = 0x71,
    VSSelect = 0x72, // Unused
    VSSetting = 0x73,
    TeamConfirm = 0x74,
    BattleModeSelect = 0x75,
    BattleVehicleSelect = 0x76,
    BattleSetting = 0x77,
    // Disabled {
    BattleCupSelect = 0x78,
    BattleCourseSelect = 0x79,
    // }
    MissionLevelSelect = 0x7a,
    MissionStageSelect = 0x7b,
    MissionInstruction = 0x7c,
    MissionDrift = 0x7d,
    MissionTutorial = 0x7e,
    ModelRender = 0x7f,
    MultiTop = 0x80,
    MultiVehicleSelect = 0x81,
    MultiDriftSelect = 0x82,
    MultiTeamSelect = 0x83,
    DirectConnection = 0x84, // Replaces WifiConnect (or something)
    WifiFirstPlay = 0x85,
    WifiDataConsent = 0x86,
    WifiDisconnect = 0x87,          // "Disconnects you"
    OnlineConnectionManager = 0x88, // Replaces unknown page
    WifiConnectionFailed = 0x89,
    WifiMultiConfirm = 0x8A,
    OnlineTop = 0x8B,        // Replaces WifiTop
    OnlineModeSelect = 0x8C, // Replaces OnlineModeSelect
    WifiFriendMenu = 0x8D,
    OnlineTeamSelect = 0x8E, // Replaces MkChannelFriendMenu
    RandomMatching = 0x8F,   // Replaces Global Search Manager
    VotingBack = 0x90,       // Replaces CountDownTimer
    WifiPlayerList = 0x91,
    Roulette = 0x92,
    Unknown93 = 0x93, // "Present in live view?"
    Unknown94 = 0x94, // "Present in online race?"
    Globe = 0x95,
    WifiFriendRoster = 0x96,
    WifiNoFriendsPopup = 0x97,
    WifiFriendRemoveConfirm = 0x98,
    WifiFriendRemoving = 0x99,
    FriendRoomRules = 0x9A, // Replaces FriendJoin
    FriendMatching = 0x9B,  // Replaces "waiting" text
    FriendRoomBack = 0x9C,  // Replaces Friend Room Manager
    FriendRoom = 0x9D,
    FriendRoomMessageSelect = 0x9E,

    ServicePackTop = 0xA2,   // Replaces ChannelTop
    StorageBenchmark = 0xA3, // Replaces ChannelRanking
    ServicePackTools = 0xA4, // Replaces ChannelGhost
    UnknownA5 = 0xA5,        // "Dummy? Seems to redirect to 0xA6"
    EnterFriendCode = 0xA6,
    GhostManager = 0xA7,
    Ranking = 0xA8,
    RankingBack = 0xAA,

    ChannelStartTimeTrial = 0xAC,
    ChannelGhostSelected = 0xAD,
    RankingTopTenDownload = 0xAE,

    UnknownB3 = 0xB3, // "Resides behind 0x4F, loads 0xB4"
    ChannelGhostList = 0xB4,
    ChannelGhostErase = 0xB5,

    CompetitionWheelOnly = 0xBA,

    Options = 0xC0,
    WifiOptions = 0xC1,
    OptionExplanation = 0xC2,
    OptionSelect2 = 0xC3,
    OptionSelect3 = 0xC4,
    RegionOption = 0xC5,
    RegionDisplayOption = 0xC6,
    OptionAwait = 0xC7,
    OptionMessage = 0xC8,
    OptionConfirm = 0xC9,
    Channel = 0xCA, // Replaces ChannelExplanation
    Update = 0xCB,  // Replaces ChannelConfirm
    OptionsBackground = 0xCC,

    MenuSettings = 0xCE,  // Replaces LicenseRecordsOverall
    SettingsPopup = 0xCF, // Replaces LicenseRecordsFavorites
    // Disabled {
    LicenseRecordsFriends = 0xD0,
    LicenseRecordsWFC = 0xD1,
    LicenseRecordsOther = 0xD2,
    // }

    Max = 0xD3,

    Ext_MinExclusive__ = 0xFF,

    // Extensions go here {
    // Do not explicitly assign values to prevent merge conflicts
    PackSelect,
    CourseDebug,
    WU8Library,
    ServicePackChannel,
    SPRankingGhostDownload,
    SPRankingTopTenDownload,
    // }

    Ext_MaxExclusive__,
};

static_assert(static_cast<size_t>(PageId::Ext_MaxExclusive__) >
        static_cast<size_t>(PageId::Ext_MinExclusive__));

constexpr size_t StandardPageCount() {
    return static_cast<size_t>(PageId::Max);
}

constexpr size_t ExtendedPageCount() {
    return static_cast<size_t>(PageId::Ext_MaxExclusive__) -
            static_cast<size_t>(PageId::Ext_MinExclusive__) - 1;
}

} // namespace UI
