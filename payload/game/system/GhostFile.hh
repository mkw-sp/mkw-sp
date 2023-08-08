#pragma once

#include "game/system/Mii.hh"
#include "game/util/Registry.hh"

#include <sp/ShaUtil.hh>

namespace System {

static const u32 MAX_GHOST_COUNT = 4096;

struct __attribute__((packed)) RawTime {
    bool isValid() const;
    u32 toMilliseconds() const;

    u16 minutes : 7;
    u8 seconds : 7;
    u16 milliseconds : 10;
};
static_assert(sizeof(RawTime) == 0x3);

struct RawGhostHeader {
    const RawTime *flap() const;

    u32 magic;

    RawTime raceTime;
    Registry::Course courseId : 6;
    u8 _pad0 : 2;

    u32 vehicleId : 6;
    u32 characterId : 6;
    u32 year : 7;
    u32 month : 4;
    u32 day : 5;
    Registry::Controller controllerId : 4;

    u8 _pad1 : 4;
    bool isCompressed : 1;
    u8 _pad2 : 2;
    u32 type : 7;
    bool driftIsAuto : 1;
    u8 _pad3 : 1;

    u16 inputsSize;
    u8 lapCount;

    RawTime lapTimes[5];

    u8 _20[0x34 - 0x20];
    u8 country;
    u8 _38[0x3c - 0x35];
    RawMii mii;
};
static_assert(sizeof(RawGhostHeader) == 0x88);

struct FooterFooter {
    u32 size;
    u32 magic;
};
static_assert(sizeof(FooterFooter) == 0x8);

struct CTGPFooter {
    u8 _00[0x48 - 0x00];
    Sha1 courseSHA1;
    u8 _5c[0x64 - 0x5c];
    f32 raceTimeDiff;
    u8 _68[0x80 - 0x68];
    f32 lapTimeDiffs[10]; // Reversed
    u8 _a8[0xc6 - 0xa8];
    u8 category;
    u8 version;
    FooterFooter footer;

    static constexpr u32 MAGIC = 0x434b4744; // CKGD
};
static_assert(sizeof(CTGPFooter) == 0xd0);

struct SPFooter {
    bool checkSize(u32 size) const;

    static void OnRaceStart(bool speedModIsEnabled, bool isVanilla, bool setInMirror);
    static void OnVanilla(bool isVanilla);
    static void OnSimplifiedControls();
    static void OnLapEnd(u32 lap, f32 timeDiff);
    static void OnUltraShortcut();
    static void OnHWG();
    static void OnWallride();
    static void OnShroom(u32 lap);
    static void OnRaceEnd(Sha1 courseSHA1);

    u32 version;
    Sha1 courseSHA1;
    f32 lapTimeDiffs[11];
    bool hasSpeedMod : 1;
    bool hasUltraShortcut : 1;
    bool hasHWG : 1;
    bool hasWallride : 1;
    u32 shroomStrategy : 15;
    bool isVanilla : 1;
    bool hasSimplifiedControls : 1;
    bool setInMirror : 1;

    static constexpr u32 MAGIC = 0x53504744; // SPGD
    static constexpr u32 VERSION = 5;
    static SPFooter s_instance;
    static u32 s_usedShrooms;
};
static_assert(sizeof(SPFooter) == 0x48);

class GhostFooter {
public:
    GhostFooter();
    GhostFooter(const u8 *raw, u32 size);
    ~GhostFooter();
    std::optional<Sha1> courseSHA1() const;
    std::optional<bool> hasSpeedMod() const;

private:
    u32 m_magic = 0;
    union {
        CTGPFooter m_ctgp;
        SPFooter m_sp;
    };
};

namespace RawGhostFile {

bool REPLACE IsValid(const u8 *raw);
bool IsValid(const u8 *raw, u32 size);
bool Decompress(const u8 *restrict src, u8 *restrict dst);

} // namespace RawGhostFile

struct Time {
    u8 _0[0x4 - 0x0];
    u16 minutes;
    u8 seconds;
    u16 milliseconds;
    u8 _a[0xc - 0xa];
};
static_assert(sizeof(Time) == 0xc);

class GhostFile {
public:
    RawMii rawMii() const;
    Time raceTime() const;
    u32 courseId() const;
    void writeHeader(RawGhostHeader *header);
    std::optional<u32> write(u8 *raw);

private:
    bool m_headerIsValid;
    u8 _01[0x18 - 0x01];
    RawMii m_rawMii;
    u8 _64[0xa4 - 0x64];
    Time m_raceTime;
    u32 m_characterId;
    u32 m_vehicleId;
    u32 m_courseId;
    u8 _bc[0xc0 - 0xbc];
    u8 m_year;
    u8 m_month;
    u8 m_day;
    u8 _c3[0xcc - 0xc3];
    u8 m_country;
    u8 _cd[0xd0 - 0xcd];
    u32 m_inputsSize;
    u8 *m_inputs;
};
static_assert(sizeof(GhostFile) == 0xd8);

class GhostGroup {
    u8 _00[0x14 - 0x00];
};
static_assert(sizeof(GhostGroup) == 0x14);

} // namespace System
