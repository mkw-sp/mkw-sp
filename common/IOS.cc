#include "IOS.hh"

#include "DCache.hh"

#include <cstring>

namespace IOS {

extern "C" volatile u32 ppcmsg;
extern "C" volatile u32 ppcctrl;
extern "C" volatile u32 armmsg;

extern "C" volatile u32 irqmask;

enum {
    X1  = 1 << 0,
    Y2  = 1 << 1,
    Y1  = 1 << 2,
    X2  = 1 << 3,
    IY1 = 1 << 4,
    IY2 = 1 << 5,
};

enum class Command : u32 {
    Open = 1,
    Close = 2,
    Read = 3,
    Write = 4,
    Seek = 5,
    Ioctl = 6,
    Ioctlv = 7,
};

struct Request {
    Command command;
    s32 result;
    s32 fd;
    union {
        struct {
            u32 path;
            Mode mode;
        } open;
        struct {
            u32 output;
            u32 outputSize;
        } read;
        struct {
            u32 input;
            u32 inputSize;
        } write;
        struct {
            u32 ioctl;
            u32 input;
            u32 inputSize;
            u32 output;
            u32 outputSize;
        } ioctl;
        struct {
            u32 ioctlv;
            u32 inputCount;
            u32 outputCount;
            u32 pairs;
        } ioctlv;
    };
    u8 user[0x40 - 0x20];
};
static_assert(sizeof(Request) == 0x40);

alignas(0x20) static Request request;

void Init() {
#ifdef SP_CHANNEL
    irqmask = 1 << 30;
    ppcctrl = X2;
#else
    //while ((ppcctrl & Y2) != Y2); HACK for Dolphin
    ppcctrl = Y2;
    ppcctrl = Y1;
#endif
}

static void Sync() {
    DCache::Flush(&request);

    ppcmsg = VirtualToPhysical(&request);
    ppcctrl = X1;

    while ((ppcctrl & Y2) != Y2);
    ppcctrl = Y2;

    u32 reply;
    do {
        while ((ppcctrl & Y1) != Y1);
        reply = armmsg;
        ppcctrl = Y1;

        ppcctrl = X2;
    } while (reply != VirtualToPhysical(&request));

    DCache::Invalidate(&request);
}

Resource::Resource(const char *path, Mode mode) {
    open(path, mode);
}

Resource::~Resource() {
    if (m_fd >= 0) {
        close();
    }
}

s32 Resource::open(const char *path, Mode mode) {
    DCache::Flush(path, strlen(path) + 1);

    request = {};
    request.command = Command::Open;
    request.open.path = VirtualToPhysical(path);
    request.open.mode = mode;

    Sync();

    m_fd = request.result;
    return request.result;
}

s32 Resource::close() {
    request = {};
    request.command = Command::Close;
    request.fd = m_fd;

    Sync();

    m_fd = -1;
    return request.result;
}

s32 Resource::ioctl(u32 ioctl, const void *input, u32 inputSize, void *output, u32 outputSize) {
    DCache::Flush(input, inputSize);
    DCache::Flush(output, outputSize);

    request = {};
    request.command = Command::Ioctl;
    request.fd = m_fd;
    request.ioctl.ioctl = ioctl;
    request.ioctl.input = VirtualToPhysical(input);
    request.ioctl.inputSize = inputSize;
    request.ioctl.output = VirtualToPhysical(output);
    request.ioctl.outputSize = outputSize;

    Sync();

    DCache::Invalidate(output, outputSize);

    return request.result;
}

s32 Resource::ioctlv(u32 ioctlv, u32 inputCount, u32 outputCount, IoctlvPair *pairs) {
    for (u32 i = 0; i < inputCount + outputCount; i++) {
        if (pairs[i].data) {
            DCache::Flush(pairs[i].data, pairs[i].size);
            pairs[i].data = reinterpret_cast<void *>(VirtualToPhysical(pairs[i].data));
        }
    }
    DCache::Flush(pairs, inputCount + outputCount);

    request = {};
    request.command = Command::Ioctlv;
    request.fd = m_fd;
    request.ioctlv.ioctlv = ioctlv;
    request.ioctlv.inputCount = inputCount;
    request.ioctlv.outputCount = outputCount;
    request.ioctlv.pairs = VirtualToPhysical(pairs);

    Sync();

    for (u32 i = inputCount; i < inputCount + outputCount; i++) {
        if (pairs[i].data) {
            pairs[i].data = PhysicalToVirtual<void *>(reinterpret_cast<u32>(pairs[i].data));
            DCache::Invalidate(pairs[i].data, pairs[i].size);
        }
    }

    return request.result;
}

bool Resource::ok() const {
    return m_fd >= 0;
}

File::File(const char *path, Mode mode) : Resource(path, mode) {}

s32 File::read(void *output, u32 outputSize) {
    request = {};
    request.command = Command::Read;
    request.fd = m_fd;
    request.read.output = VirtualToPhysical(output);
    request.read.outputSize = outputSize;

    Sync();

    DCache::Invalidate(output, outputSize);
    return request.result;
}

s32 File::write(const void *input, u32 inputSize) {
    DCache::Flush(input, inputSize);

    request = {};
    request.command = Command::Write;
    request.fd = m_fd;
    request.write.input = VirtualToPhysical(input);
    request.write.inputSize = inputSize;

    Sync();

    return request.result;
}

} // namespace IOS
