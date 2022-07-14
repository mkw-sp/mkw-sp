#pragma once

#include <Common.h>

typedef struct {
    u32 (*diskSectorSize)(void);
    bool (*diskRead)(u32 firstSector, u32 sectorCount, void *buffer);
    bool (*diskWrite)(u32 firstSector, u32 sectorCount, const void *buffer);
    bool (*diskErase)(u32 firstSector, u32 sectorCount);
    bool (*diskSync)(void);
    u32 (*getMessageId)(void);
} FATStorage;

u32 FATStorage_diskSectorSize(void);

bool FATStorage_diskRead(u32 firstSector, u32 sectorCount, void *buffer);

bool FATStorage_diskWrite(u32 firstSector, u32 sectorCount, const void *buffer);

bool FATStorage_diskErase(u32 firstSector, u32 sectorCount);

bool FATStorage_diskSync(void);
