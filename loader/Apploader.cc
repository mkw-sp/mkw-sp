#include "Apploader.hh"

#include "Console.hh"

#include <common/Clock.hh>
#include <common/ICache.hh>

namespace Apploader {

extern "C" u32 diskID[8];

struct PartitionGroup {
    u32 count;
    u32 shiftedOffset;
};
static_assert(sizeof(PartitionGroup) == 0x8);

namespace PartitionType {
    enum {
        Data = 0,
    };
} // namespace PartitionType

struct Partition {
    u32 shiftedOffset;
    u32 type;
};
static_assert(sizeof(Partition) == 0x8);

typedef void (*ApploaderReportFunc)(const char *format, ...);

typedef void (*ApploaderInitFunc)(ApploaderReportFunc);
typedef s32 (*ApploaderMainFunc)(void **dst, u32 *size, u32 *shiftedOffset);
typedef GameEntryFunc (*ApploaderCloseFunc)(void);

typedef void (*ApploaderEntryFunc)(ApploaderInitFunc *init, ApploaderMainFunc *main,
        ApploaderCloseFunc *close);

struct ApploaderHeader {
    char revision[0x10];
    ApploaderEntryFunc entry;
    u32 size;
    u32 trailer;
    u8 _1c[0x20 - 0x1c];
};
static_assert(sizeof(ApploaderHeader) == 0x20);

static std::optional<Partition> FindGamePartition(IOS::DI &di) {
    alignas(0x20) PartitionGroup groups[4];
    if (!di.readUnencrypted(groups, sizeof(groups), 0x40000)) {
        return {};
    }

    for (u32 i = 0; i < 4; i++) {
        u32 partitionCount = groups[i].count;
        u32 offset = groups[i].shiftedOffset << 2;

        if (partitionCount == 0 || partitionCount > 4 || offset == 0) {
            continue;
        }

        alignas(0x20) Partition partitions[4];
        if (!di.readUnencrypted(partitions, sizeof(partitions), offset)) {
            return {};
        }

        for (u32 j = 0; j < partitionCount; j++) {
            if (partitions[j].type == PartitionType::Data) {
                return partitions[j];
            }
        }
    }

    return {};
}

static void report(const char *UNUSED(format), ...) {}

std::optional<GameEntryFunc> LoadAndRun(IOS::DI &di) {
    if (!di.readDiskID()) {
        return {};
    }

    *reinterpret_cast<u32 *>(0x80003180) = diskID[0];
    char diskIDString[5] = { 0 };
    for (u8 i = 0; i < 4; i++) {
        diskIDString[i] = diskID[0] >> (24 - i * 8) & 0xff;
    }

    if ((diskID[0] >> 8) != (('R' << 16) | ('M' << 8) | 'C')) {
        Console::Print("This is not Mario Kart Wii (disc id ");
        Console::Print(diskIDString);
        Console::Print(").\n");

        while (di.isInserted()) {
            Clock::WaitMilliseconds(100);
        }
        return {};
    }
    Console::Print("Mario Kart Wii disc found (disc id ");
    Console::Print(diskIDString);
    Console::Print(").\n");

    std::optional<Partition> gamePartition = FindGamePartition(di);
    if (!gamePartition) {
        Console::Print("Game partition not found.\n");
        return {};
    }
    Console::Print("Found game partition.\n");

    if (!di.openPartition(gamePartition->shiftedOffset << 2)) {
        Console::Print("Failed to open game partition.\n");
        return {};
    }
    Console::Print("Successfully opened game partition.\n");

    alignas(0x20) ApploaderHeader header;
    if (!di.read(&header, sizeof(header), 0x2440)) {
        Console::Print("Failed to read apploader header.\n");
        return {};
    }
    Console::Print("Successfully read apploader header.\n");

    if (!di.read(reinterpret_cast<void *>(0x81200000), header.size + header.trailer, 0x2460)) {
        Console::Print("Failed to read apploader.\n");
        return {};
    }
    ICache::Invalidate(reinterpret_cast<void *>(0x81200000), header.size + header.trailer);
    Console::Print("Successfully read apploader.\n");

    ApploaderInitFunc init;
    ApploaderMainFunc main;
    ApploaderCloseFunc close;
    header.entry(&init, &main, &close);

    init(report);

    void *dst;
    u32 size;
    u32 shiftedOffset;
    while (main(&dst, &size, &shiftedOffset)) {
        if (!di.read(dst, size, shiftedOffset << 2)) {
            Console::Print("Failed to read dol section.\n");
            return {};
        }
        ICache::Invalidate(dst, size);
    }
    Console::Print("Successfully read dol.\n");

    return close();
}

} // namespace Apploader
