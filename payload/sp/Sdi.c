#include "Sdi.h"

#include <revolution.h>
#include <revolution/ios.h>

#include <stdalign.h>
#include <string.h>

enum {
    IOCTL_WRITE_HCR = 0x1,
    IOCTL_READ_HCR = 0x2,
    IOCTL_RESET_CARD = 0x4,
    IOCTL_SET_CLOCK = 0x6,
    IOCTL_SEND_COMMAND = 0x7,
    IOCTL_GET_STATUS = 0xb,
};

enum {
    IOCTLV_SEND_COMMAND = 0x7,
};

enum {
    STATUS_CARD_INSERTED = 1 << 0,
    STATUS_TYPE_MEMORY = 1 << 16,
    STATUS_TYPE_SDHC = 1 << 20,
};

enum {
    HCR_HOST_CONTROL_1 = 0x28,
};

enum {
    HCR_HOST_CONTROL_1_4_BIT = 1 << 1,
};

typedef struct {
    u32 reg;
    u32 _04;
    u32 _08;
    u32 size;
    u32 val;
    u32 _14;
} RegOp;

enum {
    CMD_SELECT = 7,
    CMD_SET_BLOCKLEN = 16,
    CMD_READ_MULTIPLE_BLOCK = 18,
    CMD_WRITE_MULTIPLE_BLOCK = 25,
    CMD_APP_CMD = 55,
};

enum {
    ACMD_SET_BUS_WIDTH = 6,
};

enum {
    RESPONSE_TYPE_R1 = 0x1,
    RESPONSE_TYPE_R1B = 0x2,
};

typedef struct {
    u32 command;
    u32 commandType;
    u32 responseType;
    u32 arg;
    u32 blockCount;
    u32 blockSize;
    void *buffer;
    u32 isDma;
    u32 _20;
} Request;
static_assert(sizeof(Request) == 0x24);

enum {
    SECTOR_SIZE = 512,
};

enum {
    TMP_SECTOR_COUNT = 8,
};

static s32 fd = -1;
static u16 rca = 0;
static bool isSdhc;
static alignas(0x20) u8 tmpBuffer[TMP_SECTOR_COUNT * SECTOR_SIZE];

static bool Sdi_resetCard(void) {
    alignas(0x20) u32 out;

    if (IOS_Ioctl(fd, IOCTL_RESET_CARD, NULL, 0, &out, sizeof(out)) < 0) {
        OSReport("[SDI] Failed to reset interface\n");
        return false;
    }

    OSReport("[SDI] Successfully reset interface\n");
    rca = out >> 16;
    return true;
}

static bool Sdi_getStatus(u32 *status) {
    alignas(0x20) u32 out;

    if (IOS_Ioctl(fd, IOCTL_GET_STATUS, NULL, 0, &out, sizeof(out)) < 0) {
        OSReport("[SDI] Failed to get status\n");
        return false;
    }

    OSReport("[SDI] Got status 0x%x\n", out);
    *status = out;
    return true;
}

static bool Sdi_readHcr(u8 reg, u8 size, u32 *val) {
    alignas(0x20) RegOp regOp = {
        .reg = reg,
        ._04 = 0,
        ._08 = 0,
        .size = size,
        .val = 0,
        ._14 = 0,
    };
    alignas(0x20) u32 out;

    if (IOS_Ioctl(fd, IOCTL_READ_HCR, &regOp, sizeof(regOp), &out, sizeof(out)) < 0) {
        OSReport("[SDI] Failed to read host controller register 0x%x\n", reg);
        return false;
    }

    *val = out;
    return true;
}

static bool Sdi_writeHcr(u8 reg, u8 size, u32 val) {
    alignas(0x20) RegOp regOp = {
        .reg = reg,
        ._04 = 0,
        ._08 = 0,
        .size = size,
        .val = val,
        ._14 = 0,
    };

    if (IOS_Ioctl(fd, IOCTL_WRITE_HCR, &regOp, sizeof(regOp), NULL, 0) < 0) {
        OSReport("[SDI] Failed to write to host controller register 0x%x\n", reg);
        return false;
    }

    return true;
}

static bool Sdi_setClock(u32 clock) {
    alignas(0x20) u32 in = clock;

    if (IOS_Ioctl(fd, IOCTL_SET_CLOCK, &in, sizeof(in), NULL, 0) < 0) {
        OSReport("[SDI] Failed to set clock\n");
        return false;
    }

    return true;
}

static bool Sdi_sendCommand(u32 command, u32 commandType, u32 responseType, u32 arg, u32 blockCount,
        u32 blockSize, void *buffer, u32 *response) {
    alignas(0x20) Request request = {
        .command = command,
        .commandType = commandType,
        .responseType = responseType,
        .arg = arg,
        .blockCount = blockCount,
        .blockSize = blockSize,
        .buffer = buffer,
        .isDma = !!buffer,
        ._20 = 0,
    };
    alignas(0x20) u32 out[4];

    if (buffer || isSdhc) {
        alignas(0x20) IOVector vec[] = {
            {
                .data = &request,
                .size = sizeof(request),
            },
            {
                .data = buffer,
                .size = blockCount * blockSize,
            },
            {
                .data = out,
                .size = sizeof(out),
            },
        };
        if (IOS_Ioctlv(fd, IOCTLV_SEND_COMMAND, 2, 1, vec) < 0) {
            OSReport("[SDI] Failed to send command 0x%x\n", command);
            return false;
        }
    } else {
        if (IOS_Ioctl(fd, IOCTL_SEND_COMMAND, &request, sizeof(request), &out, sizeof(out)) < 0) {
            OSReport("[SDI] Failed to send command 0x%x\n", command);
            return false;
        }
    }

    if (response) {
        *response = out[0];
    }
    return true;
}

static bool Sdi_enable4BitBus(void) {
    u32 val;
    if (!Sdi_readHcr(HCR_HOST_CONTROL_1, sizeof(u8), &val)) {
        return false;
    }

    val |= HCR_HOST_CONTROL_1_4_BIT;

    return Sdi_writeHcr(HCR_HOST_CONTROL_1, sizeof(u8), val);
}

static bool Sdi_select(void) {
    return Sdi_sendCommand(CMD_SELECT, 3, RESPONSE_TYPE_R1B, rca << 16, 0, 0, NULL, NULL);
}

static bool Sdi_deselect(void) {
    return Sdi_sendCommand(CMD_SELECT, 3, RESPONSE_TYPE_R1B, 0, 0, 0, NULL, NULL);
}

static bool Sdi_setCardBlockLength(u32 blockLength) {
    return Sdi_sendCommand(CMD_SET_BLOCKLEN, 3, RESPONSE_TYPE_R1, blockLength, 0, 0, NULL, NULL);
}

static bool Sdi_enableCard4BitBus(void) {
    if (!Sdi_sendCommand(CMD_APP_CMD, 3, RESPONSE_TYPE_R1, rca << 16, 0, 0, NULL, NULL)) {
        return false;
    }

    return Sdi_sendCommand(ACMD_SET_BUS_WIDTH, 3, RESPONSE_TYPE_R1, 0x2, 0, 0, NULL, NULL);
}

static bool Sdi_transferAligned(bool isWrite, u32 firstSector, u32 sectorCount, void *buffer) {
    u32 command = isWrite ? CMD_WRITE_MULTIPLE_BLOCK : CMD_READ_MULTIPLE_BLOCK;
    u32 firstBlock = isSdhc ? firstSector : firstSector * SECTOR_SIZE;
    return Sdi_sendCommand(command, 3, RESPONSE_TYPE_R1, firstBlock, sectorCount, SECTOR_SIZE,
            buffer, NULL);
}

static bool Sdi_transfer(bool isWrite, u32 firstSector, u32 sectorCount, void *buffer) {
    assert(buffer);

    if (!Sdi_select()) {
        return false;
    }

    if ((u32)buffer & 0x1f) {
        while (sectorCount > 0) {
            u32 chunkSectorCount = MIN(sectorCount, TMP_SECTOR_COUNT);
            if (isWrite) {
                memcpy(tmpBuffer, buffer, chunkSectorCount * SECTOR_SIZE);
            }
            if (!Sdi_transferAligned(isWrite, firstSector, chunkSectorCount, tmpBuffer)) {
                Sdi_deselect();
                return false;
            }
            if (!isWrite) {
                memcpy(buffer, tmpBuffer, chunkSectorCount * SECTOR_SIZE);
            }
            firstSector += chunkSectorCount;
            sectorCount -= chunkSectorCount;
            buffer += chunkSectorCount * SECTOR_SIZE;
        }
    } else {
        if (!Sdi_transferAligned(isWrite, firstSector, sectorCount, buffer)) {
            Sdi_deselect();
            return false;
        }
    }

    Sdi_deselect();

    return true;
}

static u32 Sdi_sectorSize(void) {
    return SECTOR_SIZE;
}

static bool Sdi_read(u32 firstSector, u32 sectorCount, void *buffer) {
    return Sdi_transfer(false, firstSector, sectorCount, buffer);
}

static bool Sdi_write(u32 firstSector, u32 sectorCount, const void *buffer) {
    return Sdi_transfer(true, firstSector, sectorCount, (void *)buffer);
}

static bool Sdi_erase(u32 firstSector, u32 sectorCount) {
    // TODO implement?
    return true;
}

static bool Sdi_sync(void) {
    // TODO implement?
    return true;
}

bool SdiStorage_init(FatStorage *fatStorage) {
    assert(fd < 0);

    fd = IOS_Open("/dev/sdio/slot0", 0);
    if (fd < 0) {
        OSReport("[SDI] Failed to open /dev/sdio/slot0: Returned error %i\n", fd);
        return false;
    } else {
        OSReport("[SDI] Successfully opened interface: ID: %i\n", fd);
    }

    if (!Sdi_resetCard()) {
        return false;
    }

    u32 status;
    if (!Sdi_getStatus(&status)) {
        return false;
    }

    if (!(status & STATUS_CARD_INSERTED)) {
        OSReport("[SDI] No card inserted\n");
        return false;
    }

    if (!(status & STATUS_TYPE_MEMORY)) {
        OSReport("[SDI] Not a memory card\n");
        return false;
    }

    isSdhc = !!(status & STATUS_TYPE_SDHC);

    if (!Sdi_enable4BitBus()) {
        OSReport("[SDI] Failed to enable 4-bit bus\n");
        return false;
    }

    if (!Sdi_setClock(1)) {
        return false;
    }

    if (!Sdi_select()) {
        return false;
    }

    if (!Sdi_setCardBlockLength(SECTOR_SIZE)) {
        Sdi_deselect();
        return false;
    }

    if (!Sdi_enableCard4BitBus()) {
        Sdi_deselect();
        return false;
    }

    Sdi_deselect();

    fatStorage->diskSectorSize = Sdi_sectorSize;
    fatStorage->diskRead = Sdi_read;
    fatStorage->diskWrite = Sdi_write;
    fatStorage->diskErase = Sdi_erase;
    fatStorage->diskSync = Sdi_sync;

    OSReport("[SDI] Successfully completed initialization\n");
    return true;
}
