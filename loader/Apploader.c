#include "Apploader.h"

#include "Cache.h"
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

    if ((diskId >> 8) != (('R' << 16) | ('M' << 8) | 'C')) {
        return false;
    }
    
    Partition gamePartition;
    if (!findGamePartition(&gamePartition)) {
        return false;
    }

    if (!Di_openPartition(gamePartition.shiftedOffset << 2)) {
        return false;
    }

    alignas(0x20) ApploaderHeader header;
    if (!Di_read(&header, sizeof(header), 0x2440)) {
        return false;
    }

    if (!Di_read((void *)0x81200000, header.size + header.trailer, 0x2460)) {
        return false;
    }
    ICInvalidateRange((void *)0x81200000, header.size + header.trailer);

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
            return false;
        }
        ICInvalidateRange(dst, size);
    }

    *gameEntry = close();

    return true;
}
