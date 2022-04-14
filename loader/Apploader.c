#include "Apploader.h"

#include "Cache.h"
#include "Console.h"
#include "Delay.h"
#include "Di.h"

#include <stdalign.h>

typedef struct {
    u32 count;
    u32 shiftedOffset;
} PartitionGroup;
static_assert(sizeof(PartitionGroup) == 0x8);

enum {
    PARTITION_TYPE_DATA = 0,
};

typedef struct {
    u32 shiftedOffset;
    u32 type;
} Partition;
static_assert(sizeof(Partition) == 0x8);

typedef void (*ApploaderReportFunc)(const char *format, ...);

typedef void (*ApploaderInitFunc)(ApploaderReportFunc);
typedef s32 (*ApploaderMainFunc)(void **dst, u32 *size, u32 *shiftedOffset);
typedef GameEntryFunc (*ApploaderCloseFunc)(void);

typedef void (*ApploaderEntryFunc)(ApploaderInitFunc *init, ApploaderMainFunc *main,
        ApploaderCloseFunc *close);

typedef struct {
    char revision[0x10];
    ApploaderEntryFunc entry;
    u32 size;
    u32 trailer;
    u8 _1c[0x20 - 0x1c];
} ApploaderHeader;
static_assert(sizeof(ApploaderHeader) == 0x20);

static bool findGamePartition(Partition *gamePartition) {
    alignas(0x20) PartitionGroup groups[4];
    if (!Di_readUnencrypted(groups, sizeof(groups), 0x40000)) {
        return false;
    }

    for (u32 i = 0; i < 4; i++) {
        u32 partitionCount = groups[i].count;
        u32 offset = groups[i].shiftedOffset << 2;

        if (partitionCount == 0 || partitionCount > 4 || offset == 0) {
            continue;
        }

        alignas(0x20) Partition partitions[4];
        if (!Di_readUnencrypted(partitions, sizeof(partitions), offset)) {
            return false;
        }

        for (u32 j = 0; j < partitionCount; j++) {
            if (partitions[j].type == PARTITION_TYPE_DATA) {
                *gamePartition = partitions[j];
                return true;
            }
        }
    }

    return false;
}

static void report(const char *format, ...) {
    (void)format;
    // Nop
}

bool Apploader_loadAndRun(GameEntryFunc *gameEntry) {
    if (!Di_readDiskId()) {
        return false;
    }

    const u32 diskId = *(u32 *)0x80000000;
    *(u32 *)0x80003180 = diskId;
    char diskIdString[5] = { 0 };
    for (u8 i = 0; i < 4; i++) {
        diskIdString[i] = diskId >> (24 - i * 8) & 0xff;
    }

    if ((diskId >> 8) != (('R' << 16) | ('M' << 8) | 'C')) {
        Console_printString("This is not Mario Kart Wii (disc id ");
        Console_printString(diskIdString);
        Console_printString(").\n");

        while (Di_isInserted()) {
            mdelay(100);
        }
    }
    Console_printString("Mario Kart Wii disc found (disc id ");
    Console_printString(diskIdString);
    Console_printString(").\n");

    Partition gamePartition;
    if (!findGamePartition(&gamePartition)) {
        Console_printString("Game partition not found.\n");
        return false;
    }
    Console_printString("Found game partition.\n");

    if (!Di_openPartition(gamePartition.shiftedOffset << 2)) {
        Console_printString("Failed to open game partition.\n");
        return false;
    }
    Console_printString("Successfully opened game partition.\n");

    alignas(0x20) ApploaderHeader header;
    if (!Di_read(&header, sizeof(header), 0x2440)) {
        Console_printString("Failed to read apploader header.\n");
        return false;
    }
    Console_printString("Successfully read apploader header.\n");

    if (!Di_read((void *)0x81200000, header.size + header.trailer, 0x2460)) {
        Console_printString("Failed to read apploader.\n");
        return false;
    }
    ICInvalidateRange((void *)0x81200000, header.size + header.trailer);
    Console_printString("Successfully read apploader.\n");

    ApploaderInitFunc init;
    ApploaderMainFunc main;
    ApploaderCloseFunc close;
    header.entry(&init, &main, &close);

    init(report);

    void *dst;
    u32 size;
    u32 shiftedOffset;
    while (main(&dst, &size, &shiftedOffset)) {
        if (!Di_read(dst, size, shiftedOffset << 2)) {
            Console_printString("Failed to read dol section.\n");
            return false;
        }
        ICInvalidateRange(dst, size);
    }
    Console_printString("Successfully read dol.\n");

    *gameEntry = close();

    return true;
}
