#include "Dolphin.hh"

namespace IOS {

namespace Ioctlv {
    enum {
        SetSpeedLimit = 4,
    };
} // namespace Ioctlv

Dolphin::Dolphin() : Resource(ALIGNED_STRING("/dev/dolphin"), Mode::None) {}

Dolphin::~Dolphin() = default;

bool Dolphin::setSpeedLimit(u32 speedLimit) {
    alignas(0x20) IoctlvPair pairs[1];
    pairs[0].data = &speedLimit;
    pairs[0].size = sizeof(speedLimit);

    s32 result = ioctlv(Ioctlv::SetSpeedLimit, 1, 0, pairs);

    return result == 0;
}

} // namespace IOS
