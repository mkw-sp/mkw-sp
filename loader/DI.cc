#include "DI.hh"

namespace IOS {

extern "C" u32 diskID[8];

namespace Ioctl {
    enum {
        ReadDiskID = 0x70,
        Read = 0x71,
        GetCoverStatus = 0x88,
        Reset = 0x8a,
        ReadUnencrypted = 0x8d,
    };
} // namespace Ioctl

namespace Ioctlv {
    enum {
        OpenPartition = 0x8b,
    };
} // namespace Ioctlv

namespace Result {
    enum {
        Success = 1,
    };
} // namespace Result

DI::DI() : Resource(ALIGNED_STRING("/dev/di"), Mode::None) {}

bool DI::readDiskID() {
    alignas(0x20) u32 in[8];
    in[0] = Ioctl::ReadDiskID << 24;

    s32 result = ioctl(Ioctl::ReadDiskID, in, sizeof(in), &diskID, sizeof(diskID));

    return result == Result::Success;
}

bool DI::readUnencrypted(void *dst, u32 size, u32 offset) {
    alignas(0x20) u32 in[8];
    in[0] = Ioctl::ReadUnencrypted << 24;
    in[1] = size;
    in[2] = offset >> 2;

    s32 result = ioctl(Ioctl::ReadUnencrypted, in, sizeof(in), dst, size);

    return result == Result::Success;
}

bool DI::openPartition(u32 offset) {
    alignas(0x20) u32 in[8];
    in[0] = Ioctlv::OpenPartition << 24;
    in[1] = offset >> 2;

    alignas(0x20) u32 out[8];

    alignas(0x20) u8 tmd[0x49e4];

    alignas(0x20) IoctlvPair pairs[5];
    pairs[0].data = in;
    pairs[0].size = sizeof(in);
    pairs[1].data = NULL;
    pairs[1].size = 0;
    pairs[2].data = NULL;
    pairs[2].size = 0;
    pairs[3].data = tmd;
    pairs[3].size = sizeof(tmd);
    pairs[4].data = out;
    pairs[4].size = sizeof(out);

    s32 result = ioctlv(Ioctlv::OpenPartition, 3, 2, pairs);

    return result == Result::Success;
}

bool DI::read(void *dst, u32 size, u32 offset) {
    alignas(0x20) u32 in[8];
    in[0] = Ioctl::Read << 24;
    in[1] = size;
    in[2] = offset >> 2;

    s32 result = ioctl(Ioctl::Read, in, sizeof(in), dst, size);

    return result == Result::Success;
}

bool DI::isInserted() {
    alignas(0x20) u32 in[8];
    in[0] = Ioctl::GetCoverStatus << 24;

    alignas(0x20) u32 out[8];

    s32 result = ioctl(Ioctl::GetCoverStatus, in, sizeof(in), out, sizeof(out));

    if (result != Result::Success) {
        return false;
    }

    return out[0] == 2;
}

bool DI::reset() {
    alignas(0x20) u32 in[8];
    in[0] = Ioctl::Reset << 24;
    in[1] = true; // Enable spinup

    s32 result = ioctl(Ioctl::Reset, in, sizeof(in), NULL, 0);

    return result == Result::Success;
}

} // namespace IOS
