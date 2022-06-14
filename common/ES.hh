#pragma once

namespace IOS::ES {

#pragma pack(push, 4)
struct Ticket {
    u32 signatureType;
    u8 _004[0x1dc - 0x004];
    u64 titleID;
    u16 accessMask;
    u8 _1e6[0x222 - 0x1e6];
    u8 contentAccessMask[512 / 8];
    u8 _262[0x2a4 - 0x262];
};
static_assert(sizeof(Ticket) == 0x2a4);

struct Content {
    u32 id;
    u16 index;
    u16 type;
    u64 size;
    u8 sha1[0x14];
};
static_assert(sizeof(Content) == 0x24);

struct Tmd {
    u32 signatureType;
    u8 _004[0x184 - 0x004];
    u64 iosID;
    u64 titleID;
    u32 titleType;
    u16 groupID;
    u8 _19a[0x19c - 0x19a];
    u16 region;
    u8 ratings[16];
    u8 _1ae[0x1dc - 0x1ae];
    u16 titleVersion;
    u16 numContents;
    u16 bootIndex;
    Content contents[512];
};
static_assert(sizeof(Tmd) == 0x49e4);
#pragma pack(pop)

} // namespace IOS::ES
