#pragma once

namespace UI {

enum class PageId {
    None = -0x1,

    RaceConfirm = 0x4b,

    Confirm = 0x52,
    Title = 0x57,
    TopMenu = 0x5a,
    LicenseSelect = 0x65,
    
    CourseSelect = 0x6f,

    TimeAttackGhostList = 0x71,

    ServicePackTop = 0xa2,

    OptionExplanation = 0xc2,
    OptionSelect2 = 0xc3,
    OptionSelect3 = 0xc4,
    OptionAwait = 0xc7,
    OptionMessage = 0xc8,
    OptionConfirm = 0xc9,
    Channel = 0xca, // Replaces ChannelExplanation
    Update = 0xcb, // Replaces ChannelConfirm

    Max = 0xd3,
};

} // namespace UI
