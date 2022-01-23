#include "GhostFile.h"

#include "Yaz.h"

#include <rfl.h>

#include <revolution.h>

#include <string.h>

// Change the maximum number of ghosts depending on the type
PATCH_S16(GhostGroup_ct, 0x56, 0); // Saved
PATCH_S16(GhostGroup_ct, 0x62, MAX_GHOST_COUNT); // Downloaded
PATCH_S16(GhostGroup_ct, 0x6e, 0); // Staff (per difficulty)
PATCH_S16(GhostGroup_ct, 0x7a, 0); // Competition

static bool RawTime_isValid(const RawTime *time) {
    if (time->minutes > 99) {
        return false;
    }

    if (time->seconds > 59) {
        return false;
    }

    if (time->milliseconds > 999) {
        return false;
    }

    return true;
}

enum {
    HEADER_MAGIC = 0x524b4744, // RKGD
};

typedef struct {
    u16 counts[3];
    u8 _6[0x8 - 0x6];
} InputsHeader;
static_assert(sizeof(InputsHeader) == 0x8);

enum {
    SP_FOOTER_VERSION = 0,
};

SpFooter s_spFooter;

void SpFooter_onRaceStart(const u32 *courseSha1, bool speedModIsEnabled) {
    memset(&s_spFooter, 0, sizeof(SpFooter));
    s_spFooter.version = SP_FOOTER_VERSION;
    memcpy(s_spFooter.courseSha1, courseSha1, sizeof(s_spFooter.courseSha1));
    s_spFooter.hasSpeedMod = speedModIsEnabled;
}

void SpFooter_onLapEnd(u32 lap, f32 timeDiff) {
    s_spFooter.lapTimeDiffs[lap] = timeDiff;
}

void SpFooter_onUltraShortcut(void) {
    s_spFooter.hasUltraShortcut = true;
}

void SpFooter_onHwg(void) {
    s_spFooter.hasHwg = true;
}

void SpFooter_onWallride(void) {
    s_spFooter.hasWallride = true;
}

void GhostFooter_init(GhostFooter *this, const u8 *raw, u32 size) {
    this->magic = 0x0;

    const RawGhostHeader *header = (RawGhostHeader *)raw;
    if (!header->isCompressed) {
        return;
    }

    u32 srcSize = *(u32 *)(raw + sizeof(RawGhostHeader));
    u32 mainSize = sizeof(RawGhostHeader) + sizeof(u32) + srcSize + sizeof(u32);

    if (mainSize + sizeof(FooterFooter) > size) {
        return;
    }

    u32 footerFooterOffset = size - sizeof(u32) - sizeof(FooterFooter);
    const FooterFooter *footerFooter = (FooterFooter *)(raw + footerFooterOffset);

    if (footerFooter->magic == CTGP_FOOTER_MAGIC && footerFooter->size == sizeof(CtgpFooter)) {
        this->magic = CTGP_FOOTER_MAGIC;
        const CtgpFooter *ctgp = (CtgpFooter *)(raw + mainSize);
        this->ctgp = *ctgp;
        return;
    }

    if (footerFooter->magic == SP_FOOTER_MAGIC && footerFooter->size >= sizeof(SpFooter)) {
        this->magic = SP_FOOTER_MAGIC;
        const SpFooter *sp = (SpFooter *)(raw + mainSize);
        this->sp = *sp;
        return;
    }
}

const u32 *GhostFooter_getCourseSha1(const GhostFooter *this) {
    switch (this->magic) {
    case CTGP_FOOTER_MAGIC:
        return this->ctgp.courseSha1;
    case SP_FOOTER_MAGIC:
        return this->sp.courseSha1;
    default:
        return NULL;
    }
}

bool GhostFooter_hasSpeedMod(const GhostFooter *this) {
    switch (this->magic) {
    case CTGP_FOOTER_MAGIC:
        return this->ctgp.category >= 4;
    case SP_FOOTER_MAGIC:
        return this->sp.hasSpeedMod;
    default:
        return NULL;
    }
}

static bool my_RawGhostFile_isValid(const u8 *raw) {
    // All checks are already done at load time by the function below
    const RawGhostHeader *header = (RawGhostHeader *)raw;
    return header->magic == HEADER_MAGIC;
}
PATCH_B(RawGhostFile_isValid, my_RawGhostFile_isValid);

static bool characterIdIsValid(u32 characterId) {
    // Non-Miis
    if (characterId < 0x18) {
        return true;
    }

    // Small Miis
    if (characterId >= 0x18 && characterId < 0x1c) {
        return true;
    }

    // Medium Miis
    if (characterId >= 0x1e && characterId < 0x22) {
        return true;
    }

    // Large Miis
    if (characterId >= 0x24 && characterId < 0x28) {
        return true;
    }

    return false;
}

static bool dateIsValid(u32 year, u32 month, u32 day) {
    if (year > 99) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    if (day < 1 || day > 31) {
        return false;
    }

    if (day == 29) {
        if (month != 2) {
            return true;
        }

        return year % 4 == 0;
    }

    if (day == 30) {
        return month != 2;
    }

    if (day == 31) {
        switch (month) {
        case 2:
        case 4:
        case 6:
        case 9:
        case 11:
            return false;
        default:
            return true;
        }
    }

    return true;
}

bool RawGhostFile_spIsValid(const u8 *raw, u32 size) {
    if (size < sizeof(RawGhostHeader)) {
        return false;
    }

    const RawGhostHeader *header = (RawGhostHeader *)raw;
    if (header->magic != HEADER_MAGIC) {
        return false;
    }

    if (!RawTime_isValid(&header->raceTime)) {
        return false;
    }

    if (header->courseId >= 0x20) {
        return false;
    }

    if (header->vehicleId >= 0x24) {
        return false;
    }

    if (!characterIdIsValid(header->characterId)) {
        return false;
    }

    if (!dateIsValid(header->year, header->month, header->day)) {
        return false;
    }

    if (header->controllerId >= 0x4) {
        return false;
    }

    if (header->isCompressed) {
        if (size < sizeof(RawGhostHeader) + sizeof(u32)) {
            return false;
        }
        u32 srcSize = *(u32 *)(raw + sizeof(RawGhostHeader));
        if (srcSize < 0x10) {
            return false;
        }
        if (size < sizeof(RawGhostHeader) + sizeof(u32) + srcSize + sizeof(u32)) {
            return false;
        }
    } else {
        if (size != 0x2800) {
            return false;
        }
    }

    if (header->lapCount != 3) {
        return false;
    }

    for (u32 i = 0; i < 3; i++) {
        if (!RawTime_isValid(&header->lapTimes[i])) {
            return false;
        }
    }

    RawMii mii = header->mii;
    mii.crc16 = 0;
    if (RFLiCalculateCRC(&mii, 0x4c) != header->mii.crc16) {
        return false;
    }

    InputsHeader inputsHeader;
    if (header->isCompressed) {
        const u8 *src = raw + sizeof(RawGhostHeader) + sizeof(u32);
        u32 srcSize = *(u32 *)(raw + sizeof(RawGhostHeader));
        u8 *dst = (u8 *)&inputsHeader;
        s32 dstSize = Yaz_getSize(src);
        if (dstSize < (s32)sizeof(inputsHeader)) {
            return false;
        }
        if (sizeof(RawGhostHeader) + (u32)dstSize + sizeof(u32) > 0x2800) {
            return false;
        }
        if (Yaz_decode(src, dst, srcSize, sizeof(inputsHeader)) != sizeof(inputsHeader)) {
            return false;
        }
    } else {
        memcpy(&inputsHeader, raw + sizeof(RawGhostHeader), sizeof(inputsHeader));
    }

    u32 maxSize = 0x2800 - (sizeof(RawGhostHeader) + sizeof(InputsHeader) + sizeof(u32));
    u32 maxCount = maxSize / sizeof(u16);
    u32 sum = 0;
    for (u32 i = 0; i < 3; i++) {
        if (inputsHeader.counts[i] > maxCount) {
            return false;
        }
        sum += inputsHeader.counts[i];
    }
    if (sum > maxCount) {
        return false;
    }

    if (!header->isCompressed) {
        u32 crc32 = *(u32 *)(raw + 0x2800 - sizeof(u32));
        return NETCalcCRC32(raw, 0x2800 - sizeof(u32)) == crc32;
    }

    u32 srcSize = *(u32 *)(raw + sizeof(RawGhostHeader));
    u32 mainSize = sizeof(RawGhostHeader) + sizeof(u32) + srcSize + sizeof(u32);
    u32 crc32 = *(u32 *)(raw + mainSize - sizeof(u32));
    if (NETCalcCRC32(raw, mainSize - sizeof(u32)) != crc32) {
        return false;
    }

    if (mainSize + sizeof(FooterFooter) > size) {
        return true;
    }

    u32 footerFooterOffset = size - sizeof(u32) - sizeof(FooterFooter);
    const FooterFooter *footerFooter = (FooterFooter *)(raw + footerFooterOffset);

    if (footerFooter->magic == CTGP_FOOTER_MAGIC && footerFooter->size == sizeof(CtgpFooter)) {
        if (mainSize + sizeof(CtgpFooter) + sizeof(u32) != size) {
            return false;
        }

        u32 crc32 = *(u32 *)(raw + size - sizeof(u32));
        return NETCalcCRC32(raw, size - sizeof(u32)) == crc32;
    }

    if (footerFooter->magic == SP_FOOTER_MAGIC && footerFooter->size >= sizeof(SpFooter)) {
        if (mainSize + footerFooter->size + sizeof(FooterFooter) + sizeof(u32) != size) {
            return false;
        }

        u32 crc32 = *(u32 *)(raw + size - sizeof(u32));
        return NETCalcCRC32(raw, size - sizeof(u32)) == crc32;
    }

    return true;
}

bool RawGhostFile_spDecompress(const u8 *restrict src, u8 *restrict dst) {
    const RawGhostHeader *srcHeader = (RawGhostHeader *)src;
    if (!srcHeader->isCompressed) {
        memcpy(dst, src, 0x2800);
        return true;
    }

    memset(dst, 0, 0x2800);
    RawGhostHeader *dstHeader = (RawGhostHeader *)dst;
    *dstHeader = *srcHeader;
    dstHeader->isCompressed = false;

    u32 srcOffset = sizeof(RawGhostHeader) + sizeof(u32);
    u32 dstOffset = sizeof(RawGhostHeader);
    u32 srcSize = *(u32 *)(src + sizeof(RawGhostHeader));
    u32 dstSize = Yaz_getSize(src + srcSize);
    Yaz_decode(src + srcOffset, dst + dstOffset, srcSize, dstSize);

    u32 crc32 = NETCalcCRC32(dst, 0x2800 - sizeof(u32));
    *(u32 *)(dst + 0x2800 - sizeof(u32)) = crc32;

    return true;
}

u32 GhostFile_spWrite(const GhostFile *this, u8 *raw) {
    memset(raw, 0, 0x2800);

    RawGhostHeader *header = (RawGhostHeader *)raw;
    GhostFile_writeHeader(this, header);
    header->inputsSize = this->inputsSize;
    header->isCompressed = true;
    u8 *dst = raw + sizeof(RawGhostHeader) + sizeof(u32);
    u32 dstSize = 0x2800;
    dstSize -= sizeof(RawGhostHeader) + 3 * sizeof(u32) + sizeof(SpFooter) + sizeof(FooterFooter);
    dstSize = Yaz_encode(this->inputs, dst, this->inputsSize, dstSize);
    if (dstSize == 0) {
        return 0;
    }
    dstSize = (dstSize + 0x3) & ~0x3;
    *(u32 *)(raw + sizeof(RawGhostHeader)) = dstSize;

    u32 mainSize = sizeof(RawGhostHeader) + sizeof(u32) + dstSize + sizeof(u32);
    u32 crc32 = NETCalcCRC32(raw, mainSize - sizeof(u32));
    *(u32 *)(raw + mainSize - sizeof(u32)) = crc32;

    *(SpFooter *)(raw + mainSize) = s_spFooter;
    FooterFooter footerFooter = { .size = sizeof(SpFooter), .magic = SP_FOOTER_MAGIC };
    *(FooterFooter *)(raw + mainSize + sizeof(SpFooter)) = footerFooter;

    u32 size = mainSize + sizeof(SpFooter) + sizeof(FooterFooter) + sizeof(u32);
    crc32 = NETCalcCRC32(raw, size - sizeof(u32));
    *(u32 *)(raw + size - sizeof(u32)) = crc32;

    return size;
}
