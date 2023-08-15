#include "GhostFile.hh"

extern "C" {
#include "GhostFile.h"
}

#include <common/Bytes.hh>
extern "C" {
#include <revolution.h>
#include <rfl.h>
#include <sp/Yaz.h>
}
#include <sp/YAZDecoder.hh>

#include <vendor/magic_enum/magic_enum.hpp>

#include <algorithm>
#include <cstring>
#include <iterator>

namespace System {

bool RawTime::isValid() const {
    if (minutes > 99) {
        return false;
    }

    if (seconds > 59) {
        return false;
    }

    if (milliseconds > 999) {
        return false;
    }

    return true;
}

u32 RawTime::toMilliseconds() const {
    return (minutes * 60 + seconds) * 1000 + milliseconds;
}

const RawTime *RawGhostHeader::flap() const {
    return std::min_element(std::begin(lapTimes), std::begin(lapTimes) + lapCount,
            [](auto &l0, auto &l1) { return l0.toMilliseconds() < l1.toMilliseconds(); });
}

bool SPFooter::checkSize(u32 size) const {
    switch (version) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return size == 0x48;
    case VERSION:
        return size == sizeof(SPFooter);
    default:
        return size >= sizeof(SPFooter);
    }
}

void SPFooter::OnRaceStart(bool speedModIsEnabled, bool isVanilla, bool setInMirror) {
    s_instance = {};
    s_instance.version = VERSION;
    s_instance.hasSpeedMod = speedModIsEnabled;
    s_instance.isVanilla = isVanilla;
    s_instance.hasSimplifiedControls = false;
    s_instance.setInMirror = setInMirror;
    s_usedShrooms = 0;
}

void SPFooter::OnVanilla(bool isVanilla) {
    s_instance.isVanilla = s_instance.isVanilla && isVanilla;
}

void SPFooter::OnSimplifiedControls() {
    s_instance.hasSimplifiedControls = true;
}

void SPFooter::OnLapEnd(u32 lap, f32 timeDiff) {
    s_instance.lapTimeDiffs[lap] = timeDiff;
}

void SPFooter::OnUltraShortcut() {
    s_instance.hasUltraShortcut = true;
}

void SPFooter::OnHWG() {
    s_instance.hasHWG = true;
}

void SPFooter::OnWallride() {
    s_instance.hasWallride = true;
}

void SPFooter::OnShroom(u32 lap) {
    if (s_usedShrooms >= 3) {
        return;
    }

    s_instance.shroomStrategy |= lap << (15 - 5 * ++s_usedShrooms);
}

void SPFooter::OnRaceEnd(Sha1 courseSHA1) {
    s_instance.courseSHA1 = courseSHA1;
}

SPFooter SPFooter::s_instance{};
u32 SPFooter::s_usedShrooms = 0;

GhostFooter::GhostFooter() = default;

GhostFooter::GhostFooter(const u8 *raw, u32 size) {
    auto *header = reinterpret_cast<const RawGhostHeader *>(raw);
    if (!header->isCompressed) {
        return;
    }

    u32 srcSize = Bytes::Read<u32>(raw, sizeof(RawGhostHeader));
    u32 mainSize = sizeof(RawGhostHeader) + sizeof(u32) + srcSize + sizeof(u32);

    if (mainSize + sizeof(FooterFooter) > size) {
        return;
    }

    u32 footerFooterOffset = size - sizeof(u32) - sizeof(FooterFooter);
    auto *footerFooter = reinterpret_cast<const FooterFooter *>(raw + footerFooterOffset);

    if (footerFooter->magic == CTGPFooter::MAGIC && footerFooter->size == sizeof(CTGPFooter)) {
        m_magic = CTGPFooter::MAGIC;
        auto *ctgp = reinterpret_cast<const CTGPFooter *>(raw + mainSize);
        m_ctgp = *ctgp;
        return;
    }

    if (footerFooter->magic == SPFooter::MAGIC && footerFooter->size >= 0x48) {
        m_magic = SPFooter::MAGIC;
        auto *sp = reinterpret_cast<const SPFooter *>(raw + mainSize);
        m_sp = *sp;
        return;
    }
}

GhostFooter::~GhostFooter() = default;

std::optional<Sha1> GhostFooter::courseSHA1() const {
    switch (m_magic) {
    case CTGPFooter::MAGIC:
        return m_ctgp.courseSHA1;
    case SPFooter::MAGIC:
        return m_sp.courseSHA1;
    default:
        return {};
    }
}

std::optional<bool> GhostFooter::hasSpeedMod() const {
    switch (m_magic) {
    case CTGPFooter::MAGIC:
        return m_ctgp.category >= 4;
    case SPFooter::MAGIC:
        return m_sp.hasSpeedMod;
    default:
        return {};
    }
}

namespace RawGhostFile {

struct InputsHeader {
    u16 counts[3];
    u8 _6[0x8 - 0x6];
};
static_assert(sizeof(InputsHeader) == 0x8);

static const u32 HEADER_MAGIC = 0x524b4744; // RKGD

bool IsValid(const u8 *raw) {
    // All checks are already done at load time by the function below
    auto *header = reinterpret_cast<const RawGhostHeader *>(raw);
    return header->magic == HEADER_MAGIC;
}

static bool DateIsValid(u32 year, u32 month, u32 day) {
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

bool IsValid(const u8 *raw, u32 size) {
    if (size < sizeof(RawGhostHeader)) {
        return false;
    }

    auto *header = reinterpret_cast<const RawGhostHeader *>(raw);
    if (header->magic != HEADER_MAGIC) {
        return false;
    }

    if (!header->raceTime.isValid()) {
        return false;
    }

    if (!Registry::IsRaceCourse(header->courseId)) {
        return false;
    }

    std::optional<Registry::Character> character =
            magic_enum::enum_cast<Registry::Character>(header->characterId);
    if (!character.has_value()) {
        return false;
    }

    std::optional<Registry::Vehicle> vehicle =
            magic_enum::enum_cast<Registry::Vehicle>(header->vehicleId);
    if (!vehicle.has_value()) {
        return false;
    }

    if (!Registry::IsCombinationValid(character.value(), vehicle.value())) {
        return false;
    }

    if (!DateIsValid(header->year, header->month, header->day)) {
        return false;
    }

    if (!IsControllerValid(header->controllerId)) {
        return false;
    }

    if (header->isCompressed) {
        if (size < sizeof(RawGhostHeader) + sizeof(u32)) {
            return false;
        }
        u32 srcSize = Bytes::Read<u32>(raw, sizeof(RawGhostHeader));
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
        if (!header->lapTimes[i].isValid()) {
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
        u32 srcSize = Bytes::Read<u32>(raw, sizeof(RawGhostHeader));
        auto *dst = reinterpret_cast<u8 *>(&inputsHeader);
        auto dstSize = SP::YAZDecoder::GetDecodedSize(src, srcSize);
        if (!dstSize || *dstSize < sizeof(inputsHeader)) {
            return false;
        }
        if (sizeof(RawGhostHeader) + *dstSize + sizeof(u32) > 0x2800) {
            return false;
        }
        dstSize = SP::YAZDecoder::Decode(src, srcSize, dst, sizeof(inputsHeader));
        if (!dstSize || *dstSize != sizeof(inputsHeader)) {
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
        u32 crc32 = Bytes::Read<u32>(raw, 0x2800 - sizeof(u32));
        return NETCalcCRC32(raw, 0x2800 - sizeof(u32)) == crc32;
    }

    u32 srcSize = Bytes::Read<u32>(raw, sizeof(RawGhostHeader));
    u32 mainSize = sizeof(RawGhostHeader) + sizeof(u32) + srcSize + sizeof(u32);
    u32 crc32 = Bytes::Read<u32>(raw, mainSize - sizeof(u32));
    if (NETCalcCRC32(raw, mainSize - sizeof(u32)) != crc32) {
        return false;
    }

    if (mainSize + sizeof(FooterFooter) > size) {
        return true;
    }

    u32 footerFooterOffset = size - sizeof(u32) - sizeof(FooterFooter);
    auto *footerFooter = reinterpret_cast<const FooterFooter *>(raw + footerFooterOffset);

    if (footerFooter->magic == CTGPFooter::MAGIC && footerFooter->size == sizeof(CTGPFooter)) {
        if (mainSize + sizeof(CTGPFooter) + sizeof(u32) != size) {
            return false;
        }

        u32 crc32 = Bytes::Read<u32>(raw, size - sizeof(u32));
        return NETCalcCRC32(raw, size - sizeof(u32)) == crc32;
    }

    if (footerFooter->magic == SPFooter::MAGIC && footerFooter->size >= 0x48) {
        if (mainSize + footerFooter->size + sizeof(FooterFooter) + sizeof(u32) != size) {
            return false;
        }

        auto *sp = reinterpret_cast<const SPFooter *>(raw + mainSize);
        if (!sp->checkSize(footerFooter->size)) {
            return false;
        }

        u32 crc32 = Bytes::Read<u32>(raw, size - sizeof(u32));
        return NETCalcCRC32(raw, size - sizeof(u32)) == crc32;
    }

    return true;
}

bool Decompress(const u8 *restrict src, u8 *restrict dst) {
    auto *srcHeader = reinterpret_cast<const RawGhostHeader *>(src);
    if (!srcHeader->isCompressed) {
        memcpy(dst, src, 0x2800);
        return true;
    }

    memset(dst, 0, 0x2800);
    auto *dstHeader = reinterpret_cast<RawGhostHeader *>(dst);
    *dstHeader = *srcHeader;
    dstHeader->isCompressed = false;

    u32 srcOffset = sizeof(RawGhostHeader) + sizeof(u32);
    u32 dstOffset = sizeof(RawGhostHeader);
    u32 srcSize = Bytes::Read<u32>(src, sizeof(RawGhostHeader));
    auto dstSize = SP::YAZDecoder::GetDecodedSize(src + srcOffset, srcSize);
    if (!dstSize) {
        return false;
    }
    if (SP::YAZDecoder::Decode(src + srcOffset, srcSize, dst + dstOffset, *dstSize) != dstSize) {
        return false;
    }

    u32 crc32 = NETCalcCRC32(dst, 0x2800 - sizeof(u32));
    Bytes::Write<u32>(dst, 0x2800 - sizeof(u32), crc32);

    return true;
}

} // namespace RawGhostFile

RawMii GhostFile::rawMii() const {
    return m_rawMii;
}

Time GhostFile::raceTime() const {
    return m_raceTime;
}

u32 GhostFile::courseId() const {
    return m_courseId;
}

std::optional<u32> GhostFile::write(u8 *raw) {
    memset(raw, 0, 0x2800);

    auto *header = reinterpret_cast<RawGhostHeader *>(raw);
    writeHeader(header);
    header->type = GHOST_TYPE_EXPERT_STAFF;
    header->inputsSize = m_inputsSize;
    header->isCompressed = true;
    u8 *dst = raw + sizeof(RawGhostHeader) + sizeof(u32);
    u32 dstSize = 0x2800;
    dstSize -= sizeof(RawGhostHeader) + 3 * sizeof(u32) + sizeof(SPFooter) + sizeof(FooterFooter);
    dstSize = Yaz_encode(m_inputs, dst, m_inputsSize, dstSize);
    if (dstSize == 0) {
        return 0;
    }
    dstSize = (dstSize + 0x3) & ~0x3;
    Bytes::Write<u32>(raw, sizeof(RawGhostHeader), dstSize);

    u32 mainSize = sizeof(RawGhostHeader) + sizeof(u32) + dstSize + sizeof(u32);
    u32 crc32 = NETCalcCRC32(raw, mainSize - sizeof(u32));
    Bytes::Write<u32>(raw, mainSize - sizeof(u32), crc32);

    *reinterpret_cast<SPFooter *>(raw + mainSize) = SPFooter::s_instance;
    FooterFooter footerFooter = {.size = sizeof(SPFooter), .magic = SPFooter::MAGIC};
    *(FooterFooter *)(raw + mainSize + sizeof(SPFooter)) = footerFooter;

    u32 size = mainSize + sizeof(SPFooter) + sizeof(FooterFooter) + sizeof(u32);
    crc32 = NETCalcCRC32(raw, size - sizeof(u32));
    Bytes::Write<u32>(raw, size - sizeof(u32), crc32);

    return size;
}

} // namespace System

extern "C" {

void SPFooter_OnLapEnd(u32 lap, f32 timeDiff) {
    System::SPFooter::OnLapEnd(lap, timeDiff);
}

void SPFooter_OnUltraShortcut(void) {
    System::SPFooter::OnUltraShortcut();
}

void SPFooter_OnHWG(void) {
    System::SPFooter::OnHWG();
}

void SPFooter_OnWallride(void) {
    System::SPFooter::OnWallride();
}

void SPFooter_OnShroom(u32 lap) {
    System::SPFooter::OnShroom(lap);
}

bool RawGhostFile_IsValid(const u8 *raw) {
    return System::RawGhostFile::IsValid(raw);
}
}
