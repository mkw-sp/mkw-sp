#pragma once

namespace UI {

enum class PageId {
    None = -0x1,

    TARace = 0xd,

    ConfirmQuit = 0x2c,

    GhostReplayRace = 0x37,

    RaceConfirm = 0x4b,

    MenuMessage = 0x51,
    Confirm = 0x52,
    Title = 0x57,
    TopMenu = 0x5a,
    LicenseSelect = 0x65,
    LicenseSettings = 0x67,

    CourseSelect = 0x6f,
    TimeAttackTop = 0x70,
    TimeAttackGhostList = 0x71,

    ServicePackTop = 0xa2, // Replaces ChannelTop
    ChannelRanking = 0xa3,
    ChannelGhost = 0xa4,

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
    LicenseRecordsFavorites = 0xcf,
    LicenseRecordsFriends = 0xd0,
    LicenseRecordsWFC = 0xd1,
    LicenseRecordsOther = 0xd2,

    Max = 0xd3,
};

} // namespace UI
