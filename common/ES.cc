#include "ES.hh"

namespace IOS {

namespace Ioctlv {
    enum {
        LaunchTitle = 0x8,
        GetTicketViewCount = 0x12,
        GetTicketViews = 0x13,
    };
} // namespace Ioctlv


ES::ES() : Resource(ALIGNED_STRING("/dev/es"), Mode::None) {}

ES::~ES() = default;

std::optional<u32> ES::getTicketViewCount(u64 titleID) {
    u32 count;

    alignas(0x20) IoctlvPair pairs[2];
    pairs[0].data = &titleID;
    pairs[0].size = sizeof(titleID);
    pairs[1].data = &count;
    pairs[1].size = sizeof(count);

    s32 result = ioctlv(Ioctlv::GetTicketViewCount, 1, 1, pairs);

    if (result < 0) {
        return {};
    }
    return count;
}

bool ES::getTicketViews(u64 titleID, u32 count, TicketView *views) {
    alignas(0x20) IoctlvPair pairs[3];
    pairs[0].data = &titleID;
    pairs[0].size = sizeof(titleID);
    pairs[1].data = &count;
    pairs[1].size = sizeof(count);
    pairs[2].data = views;
    pairs[2].size = count * sizeof(TicketView);

    s32 result = ioctlv(Ioctlv::GetTicketViews, 2, 1, pairs);

    return result >= 0;
}

bool ES::launchTitle(u64 titleID, TicketView *view) {
    alignas(0x20) IoctlvPair pairs[2];
    pairs[0].data = &titleID;
    pairs[0].size = sizeof(titleID);
    pairs[1].data = view;
    pairs[1].size = sizeof(TicketView);

    bool result = ioctlvReboot(Ioctlv::LaunchTitle, 2, pairs);

    m_fd = -1;

    return result;
}

} // namespace IOS
