#pragma once

namespace UI {

enum class SectionId {
    Restart = 0x5,

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

    MRReplay = 0x2c,
    TournamentReplay = 0x2d,
    GPReplay = 0x2e,
    TAReplay = 0x2f,

    GhostTA = 0x30,
    GhostTAOnline = 0x31,

    GhostReplayChannel = 0x32,
    GhostReplayDownload = 0x33,
    GhostReplay = 0x34,

    TitleFromMenu = 0x41,
    TitleFromOptions = 0x43,
    MiiSelectCreate = 0x45,
    MiiSelectChange = 0x46,
    LicenseSettings = 0x47,

    Single = 0x48,
    SingleChangeDriver = 0x49,
    SingleChangeCourse = 0x4a,
    SingleChangeMission = 0x4d,

    SingleChangeGhostData = 0x4e, // Replaces SingleMkChannelGhost
    ServicePack = 0x7a, // Replaces Channel

    Unlock0 = 0x90,
    Unlock1 = 0x91,
    Unlock2 = 0x92,
    Unlock3 = 0x93,

    Max = 0x95,
};

} // namespace UI
