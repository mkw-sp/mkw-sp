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

    CompetitionPersonalLeaderboard = 0x35,

    GhostReplayRace = 0x37,
    GpReplayPauseMenu = 0x38,
    TaReplayPauseMenu = 0x39,

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
    LicenseSelect = 0x65,
    LicenseSettingsTop = 0x67,
    SingleTop = 0x69,
    CharacterSelect = 0x6B,
    VehicleSelect = 0x6C,
    DriftSelect = 0x6D,
    CupSelect = 0x6E,
    CourseSelect = 0x6f,
    TimeAttackTop = 0x70,
    TimeAttackGhostList = 0x71,
    VsModeSelect = 0x72,
    RaceRules = 0x73,
    TeamsOverview = 0x74,
    BattleModeSelect = 0x75,
    BattleVehicleSelect = 0x76,
    BattleRules = 0x77,
    BattleCupSelect = 0x78,
    BattleCourseSelect = 0x79,
    MissionLevelSelect = 0x7a,
    MissionStageSelect = 0x7b,
    MissionPrompt = 0x7c,
    MissionDrift = 0x7d,
    MissionTutorial = 0x7e,

    OnlineTop = 0x8b, // Replaces WifiTop
    Globe = 0x95,
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

    Settings = 0xce, // Replaces LicenseRecordsOverall
    // Disabled {
    LicenseRecordsFavorites = 0xcf,
    LicenseRecordsFriends = 0xd0,
    LicenseRecordsWFC = 0xd1,
    LicenseRecordsOther = 0xd2,
    // }

    Max = 0xd3,
};

} // namespace UI
