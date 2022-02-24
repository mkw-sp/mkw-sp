#pragma once

#include "Storage.h"

typedef struct {
    bool (*read)(u32 firstSector, u32 sectorCount, void *buffer);
    bool (*write)(u32 firstSector, u32 sectorCount, const void *buffer);
    bool (*erase)(u32 firstSector, u32 sectorCount);
    bool (*sync)(void);
} FatStorage;

bool FatStorage_init(Storage *storage);

bool FatStorage_read(u32 firstSector, u32 sectorCount, void *buffer);

bool FatStorage_write(u32 firstSector, u32 sectorCount, const void *buffer);

bool FatStorage_erase(u32 firstSector, u32 sectorCount);

bool FatStorage_sync(void);
