#pragma once

namespace UI {

enum class SectionId {
    None = -1,
    PowerOffWii = 0x0,

    MkChannelInstaller = 0x2,
    WiiMenuRegionSelect = 0x3,
    ReturnToWiiMenu = 0x4,
    Restart = 0x5,
    Thumbnails = 0x6, // Was unused

    CreateSave = 0x10,
    CorruptSave = 0x11,
    CountryChanged = 0x12,
    CannotSave = 0x13,
    CannotAccessNand = 0x14,
    CannotAccessMiis = 0x15,

    ESRBWarning = 0x16,
    FPSWarning1 = 0x17,
    FpsWarning2 = 0x18,

    GPDemo = 0x19,
    VSDemo = 0x1A,
    BTDemo = 0x1B,
    MRBossDemo = 0x1C,
    CoBossDemo = 0x1D,

    GP = 0x1E,
    TA = 0x1F,

    VS1P = 0x20,
    VS2P = 0x21,
    VS3P = 0x22,
    VS4P = 0x23,

    TeamVS1P = 0x24,
    TeamVS2P = 0x25,
    TeamVS3P = 0x26,
    TeamVS4P = 0x27,

    Battle1P = 0x28,
    Battle2P = 0x29,
    Battle3P = 0x2A,
    Battle4P = 0x2B,

    MR = 0x2C,

    TournamentReplay = 0x2D,
    GPReplay = 0x2E,
    TAReplay = 0x2F,

    GhostTA = 0x30,
    GhostTAOnline = 0x31,

    GhostReplayChannel = 0x32,
    GhostReplayDownload = 0x33,
    GhostReplay = 0x34,

    AwardsGP = 0x35,
    AwardsVS = 0x36,
    Unknown37 = 0x37, // Seems to be the same as 0x36
    AwardsBT = 0x38,

    // Unreachable, due to no GP {
    CreditsP1 = 0x39,
    CreditsP1True = 0x3A,
    CreditsP2 = 0x3B,
    CreditsP2True = 0x3C,
    Congratulations = 0x3D,
    CongratulationsTrue = 0x3E,
    // }

    TitleFromBoot = 0x3F,
    TitleFromReset = 0x40,
    TitleFromMenu = 0x41,
    TitleFromNewLicence = 0x42,
    TitleFromOptions = 0x43,

    Demo = 0x44,

    MiiSelectCreate = 0x45,
    MiiSelectChange = 0x46,
    LicenseSettings = 0x47,

    Single = 0x48,
    SingleChangeDriver = 0x49,
    SingleChangeCourse = 0x4A,
    SingleSelectVSCourse = 0x4B,
    SingleSelectBTCourse = 0x4C,
    SingleChangeMission = 0x4D,
    SingleChangeGhostData = 0x4E, // Replaces SingleMkChannelGhost
    SingleChannelLeaderboardChallenge = 0x4F,
    SingleGhostListChallenge = 0x50,

    // From Mario Kart Channel {
    SendGhostDataToFriend = 0x51,
    ChallengeGhost = 0x52,
    WatchReplay = 0x53,
    // }

    Multi = 0x54,

    OnlineSingle = 0x55, // Replaces WifiSingle
    WifiSingleDisconnected = 0x56,
    WifiSingleFriendList = 0x57,
    WifiSingleVsVoting = 0x58,
    WifiSingleBtVoting = 0x59,

    OnlineMultiConfigure = 0x5A, // Replaces WifiMultiMiiConfigure
    OnlineMulti = 0x5B,          // Replaces WifiMulti
    WifiMultiDisconnected = 0x5C,
    WifiMultiFriendList = 0x5D,
    WifiMultiVsVoting = 0x5E,
    WifiMultiBtVoting = 0x5F,

    Voting1PVS = 0x60,
    Voting1PTeamVS = 0x61, // Unused
    Voting1PBalloon = 0x62,
    Voting1PCoin = 0x63,
    Voting2PVS = 0x64,
    Voting2PTeamVS = 0x65, // Unused
    Voting2PBalloon = 0x66,
    Voting2PCoin = 0x67,

    WifiVS = 0x68,
    WifiMultiVS = 0x69,
    WifiVSSpectate = 0x6A,
    WifiVSMultiSpectate = 0x6B,

    WifiBT = 0x6C,
    WifiMultiBT = 0x6D,
    WifiBTSpectate = 0x6E,
    WifiBTMultiSpectate = 0x6F,

    OnlineFriend1PVS = 0x70,
    OnlineFriend1PTeamVS = 0x71, // Unused
    OnlineFriend1PBalloon = 0x72,
    OnlineFriend1PCoin = 0x73,

    OnlineFriend2PVS = 0x74,
    OnlineFriend2PTeamVS = 0x75, // Unused
    OnlineFriend2PBalloon = 0x76,
    OnlineFriend2PCoin = 0x77,

    OnlineDisconnected = 0x78,
    OnlineDisconnectedGeneric = 0x79,

    ServicePack = 0x7A, // Replaces Channel
    ChannelFromTimeTrials = 0x7B,
    Rankings = 0x7D,
    // ^^ All channel sections vv
    // 0x84 replaces OnlineServer

    Unknown82 = 0x82,
    CompetitionChannel = 0x85,
    CompetitionChangeCharacter = 0x86,
    CompetitionSubmitRecord = 0x87,
    CompetitionWheelOnly = 0x88,
    CompetitionWheelOnlyBosses = 0x89,

    InviteFriend = 0x8A,
    ChannelDownloadData = 0x8B,

    Options = 0x8C,
    AddMkChannel = 0x8D,
    EnableMessageService = 0x8E,

    Unlock0 = 0x90,
    Unlock1 = 0x91,
    Unlock2 = 0x92,
    Unlock3 = 0x93,
    MissionMenu = 0x94,

    Max = 0x95,
    TrialMax = 0xB2,
    // Sections 0x95 - 0xB2 are reserved and cannot be used here

    // Extensions go here {
    // Do not explicitly assign values to prevent merge conflicts
    WU8Library,
    ServicePackChannel,
    ServicePackRankings,
    // }
};

} // namespace UI
