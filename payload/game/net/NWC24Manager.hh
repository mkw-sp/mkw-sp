#pragma once

#include <Common.hh>

namespace Net {

struct RKCT_Header {
    char fileMagic[4];
    u32 dataLength;
    u16 _08;
    u16 headersLength;
    u32 unknown;
};
static_assert(sizeof(RKCT_Header) == 0x10);

struct RKCO_Header {
    char sectionMagic[4];
    u16 gameMode;
    u8 courseId;
    u8 engineClass;
    u8 laps;
    u8 padding1;
    u16 vehicleRestriction;
    u16 cameraAngle;
    u16 minimapObject;
    u16 empty1;
    u16 empty2;
    u16 cannonFlag;
    u16 cpuPlayers;
    u8 driverVehicleSettings[22];
    u8 controller;
    u8 padding2;
    u16 commonFiles;
    u16 score;
    u8 introSettings;
    u8 padding3;
    u16 unknown2;
    u16 unknown3;
    u16 checksum;
};
static_assert(sizeof(RKCO_Header) == 0x3C);

struct RKC {
    RKCT_Header header;
    RKCO_Header RKCO;
    u32 yazMagic;
    u32 yazDecompSize;
    u32 yazReserved;
    // ...
};

class CompetitionInfo {
public:
    bool m_hasRKCT = false;
    u8 _01[0x4 - 0x1];
    RKC *m_RKCT = nullptr;
    u32 m_saveMgrArg;
    u32 m_type;
    u32 m_miiName;
    u32 m_miiNameLen;
    u8 m_saveMgrFlag;
    u8 _19[0x1c - 0x19];
};

static_assert(sizeof(CompetitionInfo) == 0x1c);

class NW4C24Manager {
public:
    static NW4C24Manager *InitStaticInstance();

    bool getCompetitionInfo(CompetitionInfo &info);
};

} // namespace Net
