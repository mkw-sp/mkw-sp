#pragma once

#include "Storage.h"

typedef struct {
    u32 (*diskSectorSize)(void);
    bool (*diskRead)(u32 firstSector, u32 sectorCount, void *buffer);
    bool (*diskWrite)(u32 firstSector, u32 sectorCount, const void *buffer);
    bool (*diskErase)(u32 firstSector, u32 sectorCount);
    bool (*diskSync)(void);
} FatStorage;

bool FatStorage_init(Storage *storage);

u32 FatStorage_diskSectorSize(void);

bool FatStorage_diskRead(u32 firstSector, u32 sectorCount, void *buffer);

bool FatStorage_diskWrite(u32 firstSector, u32 sectorCount, const void *buffer);

bool FatStorage_diskErase(u32 firstSector, u32 sectorCount);

bool FatStorage_diskSync(void);
