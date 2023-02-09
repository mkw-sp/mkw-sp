#pragma once

namespace UI {

enum class SectionId {
    None = -1,

    Restart = 0x5,

    Thumbnails = 0x6, // Was unused

    GPDemo = 0x19,
    VSDemo = 0x1a,
    BTDemo = 0x1b,
    MRBossDemo = 0x1c,

    GP = 0x1e,
    TA = 0x1f,

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
    Battle3P = 0x2a,
    Battle4P = 0x2b,

    MR = 0x2c,

    TournamentReplay = 0x2d,
    GPReplay = 0x2e,
    TAReplay = 0x2f,

    GhostTA = 0x30,
    GhostTAOnline = 0x31,

    GhostReplayChannel = 0x32,
    GhostReplayDownload = 0x33,
    GhostReplay = 0x34,

    AwardsGP = 0x35,
    AwardsVS = 0x36,
    AwardsBT = 0x38,

    TitleFromBoot = 0x3f,
    TitleFromMenu = 0x41,
    TitleFromOptions = 0x43,
    MiiSelectCreate = 0x45,
    MiiSelectChange = 0x46,
    LicenseSettings = 0x47,

    Single = 0x48,
    SingleChangeDriver = 0x49,
    SingleChangeCourse = 0x4a,
    SingleSelectVSCourse = 0x4b,
    SingleSelectBTCourse = 0x4c,
    SingleChangeMission = 0x4d,

    SingleChangeGhostData = 0x4e, // Replaces SingleMkChannelGhost

    Multi = 0x54,

    OnlineSingle = 0x55, // Replaces WifiSingle
    OnlineMultiConfigure = 0x5a, // Replaces WifiMultiConfigure
    OnlineMulti = 0x5b, // Replaces WifiMulti

    Voting1PVS = 0x60,
    // 0x61 replaces VotingServer, free to use!
    Voting1PBalloon = 0x62,
    Voting1PCoin = 0x63,
    Voting2PVS = 0x64,
    // 0x65 replaces deprecated Voting2PTeamVS, free to use!
    Voting2PBalloon = 0x66,
    Voting2PCoin = 0x67,

    Online1PVS = 0x70,
    // 0x71 replaces Wifi1PTeamVS/OnlineServerVS, free to use!

    ServicePack = 0x7a, // Replaces Channel

    // 0x84 replaces OnlineServer, free to use!

    Unlock0 = 0x90,
    Unlock1 = 0x91,
    Unlock2 = 0x92,
    Unlock3 = 0x93,

    Max = 0x95,
};

} // namespace UI
