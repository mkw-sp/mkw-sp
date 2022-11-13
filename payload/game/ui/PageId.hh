#pragma once

namespace UI {

enum class PageId {
    None = -0x1,

    TARace = 0xd,

    GpPauseMenu = 0x17,
    VsPauseMenu = 0x18,
    TaPauseMenu = 0x19,
    BattlePauseMenu = 0x1A,
    MrPauseMenu = 0x1B,
    TaGhostPauseMenu = 0x1C,

    GhostReplayPauseMenu = 0x1F,

    AfterTaMenu = 0x21,
    AfterVsMenu = 0x22,
    AfterMrMenu = 0x25,
    AfterTournamentMenu = 0x26,
    SendTournamentRecord = 0x2A,
    CheckRankings = 0x2B,
    ConfirmQuit = 0x2C,

    ResultRaceUpdate = 0x2f,
    ResultRaceTotal = 0x30,
    ResultTeamVSTotal = 0x32,

    CompetitionPersonalLeaderboard = 0x35,

    GhostReplayRace = 0x37,
    GpReplayPauseMenu = 0x38,
    TaReplayPauseMenu = 0x39,

    Award = 0x3c,

    OnlinePleaseWait = 0x48,
    RaceConfirm = 0x4B,
    MessagePopup = 0x4D,
    YesNoPopup = 0x4E,
    ReadingGhostData = 0x4F,
    ConnectingNintendoWfc = 0x50,
    MenuMessage = 0x51,
    Confirm = 0x52,
    Title = 0x57,
    TopMenu = 0x5A,
    Model = 0x5C,
    Obi = 0x5E,
    MultiPadRegister = 0x61,
    LicenseSelect = 0x65,
    LicenseSettingsTop = 0x67,
    SingleTop = 0x69,
    CharacterSelect = 0x6B,
    VehicleSelect = 0x6C,
    DriftSelect = 0x6D,
    CourseSelect = 0x6E, // Replaces CupSelect
    //CourseSelect = 0x6f,
    TimeAttackTop = 0x70,
    TimeAttackGhostList = 0x71,
    VSSelect = 0x72,
    VSSetting = 0x73,
    TeamConfirm = 0x74,
    BattleModeSelect = 0x75,
    BattleVehicleSelect = 0x76,
    BattleSetting = 0x77,
    BattleCupSelect = 0x78,
    BattleCourseSelect = 0x79,
    MissionLevelSelect = 0x7a,
    MissionStageSelect = 0x7b,
    MissionPrompt = 0x7c,
    MissionDrift = 0x7d,
    MissionTutorial = 0x7e,
    ModelRender = 0x7f,
    MultiTop = 0x80,
    MultiVehicleSelect = 0x81,
    MultiTeamSelect = 0x83,
    DirectConnection = 0x84, // Replaces WifiConnect (or something)

    OnlineTeamSelect = 0x88,
    OnlineTop = 0x8b, // Replaces WifiTop
    VotingBack = 0x90, // Replaces CountDownTimer
    Roulette = 0x92,
    Globe = 0x95,
    FriendRoomRules = 0x9a, // Replaces FriendJoin
    FriendMatching = 0x9b,
    FriendRoomBack = 0x9c,
    FriendRoom = 0x9d,
    FriendRoomMessageSelect = 0x9e,

    ServicePackTop = 0xa2, // Replaces ChannelTop
    StorageBenchmark = 0xa3, // Replaces ChannelRanking
    ServicePackTools = 0xa4, // Replaces ChannelGhost

    GhostManager = 0xa7,

    OptionExplanation = 0xc2,
    OptionSelect2 = 0xc3,
    OptionSelect3 = 0xc4,
    OptionAwait = 0xc7,
    OptionMessage = 0xc8,
    OptionConfirm = 0xc9,
    Channel = 0xca, // Replaces ChannelExplanation
    Update = 0xcb, // Replaces ChannelConfirm

    MenuSettings = 0xce, // Replaces LicenseRecordsOverall
    SettingsPopup = 0xcf, // Replaces LicenseRecordsFavorites
    // Disabled {
    LicenseRecordsFriends = 0xd0,
    LicenseRecordsWFC = 0xd1,
    LicenseRecordsOther = 0xd2,
    // }

    Max = 0xd3,
};

} // namespace UI
