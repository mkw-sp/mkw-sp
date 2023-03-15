#pragma once

#include <common/TVec3.hh>

namespace System {

struct MapdataKartPoint {
    struct SData {
        Vec3 pos;
        Vec3 rot;
        s16 id;
        u8 _1a[0x1c - 0x1a];
    };
    static_assert(sizeof(SData) == 0x1c);

    void getTransform(Vec3 *pos, Vec3 *rot, u32 rank, u32 playerCount);

    MapdataKartPoint(SData *sdata) : m_data(sdata) {}
    const SData *m_data;
    // ...
};

struct MapdataCheckPath {
    struct SData {
        u8 start; //!< [+0x00]
        u8 size;  //!< [+0x01]

        u8 last[6]; //!< [+0x02]
        u8 next[6]; //!< [+0x08]
    };
    MapdataCheckPath(SData *sdata) : m_data(sdata) {}
    const SData *m_data;
    // ...
};

struct MapdataCheckPoint {
    struct SData {
        Vec2<float> left;  //!< [+0x00] First point of the checkpoint.
        Vec2<float> right; //!< [+0x08] Second point of the checkpoint.
        u8 jugemIndex;     //!< [+0x10] Respawn point.
        u8 lapCheck;       //!< [+0x11] 0 - start line, 1-254 - key checkpoints, 255 -
                           //!< normal
        u8 prevPt;         //!< [+0x12] Last checkpoint. 0xFF -> sequence edge
        u8 nextPt;         //!< [+0x13] Next checkpoint. 0xFF -> sequence edge
    };

    MapdataCheckPoint(SData *sdata) : m_data(sdata) {}
    const SData *m_data;
    // ...
};

struct MapdataJugemPoint {
    struct SData {
        Vec3 position, rotation;
        u16 id;
        s16 range;
    };
    MapdataJugemPoint(SData *sdata) : m_data(sdata) {}
    const SData *m_data;
    // ...
};

//! @brief Section Header that starts all .kmp file sections
struct ResSectionHeader {
    u32 magic;  //!< [+0x00] Four characters
    u16 nEntry; //!< [+0x04] Number of entries in the section
    union {
        u16 userData; //!< [+0x06] The POTI section stores the total number of
                      //!< points of all routes. The CAME section store different
                      //!< values.
        struct {
            s8 openingPanIdx; //!< [+0x06] CAME opening pan idx.
            s8 videoPanIdx;   //!< [+0x07] CAME video pan idx.
        } cameraData;
    };
};

// hacky res definition
template <typename T, typename TData>
struct MapdataAccessorBase {
    T **m_entryAccessors;        //!< [+0x00]
    u16 m_numEntries;            //!< [+0x04]
    ResSectionHeader *mpSection; //!< [+0x08]

    const TData *cdata(size_t i) const {
        if (i < 0 || i > m_numEntries) {
            return nullptr;
        }
        return m_entryAccessors[i]->m_data;
    }
};

struct MapdataKartPointAccessor
    : public MapdataAccessorBase<MapdataKartPoint, MapdataKartPoint::SData> {
    // ...
};
struct MapdataCheckPointAccessor
    : public MapdataAccessorBase<MapdataCheckPoint, MapdataCheckPoint::SData> {
    // ...
};
struct MapdataCheckPathAccessor
    : public MapdataAccessorBase<MapdataCheckPath, MapdataCheckPath::SData> {
    // ...
};
struct MapdataJugemPointAccessor
    : public MapdataAccessorBase<MapdataJugemPoint, MapdataJugemPoint::SData> {
    // ...
};

class CourseMap {
public:
    virtual void vf00() = 0;
    virtual void vf04() = 0;
    virtual void dt() = 0;

    const MapdataKartPointAccessor *kartPoint() const;

    static CourseMap *Instance();

    void *mpCourse;

    MapdataKartPointAccessor *m_kartPoint;
    void *mpEnemyPath;
    void *mpEnemyPoint;
    void *mpItemPath;
    void *mpItemPoint;
    MapdataCheckPathAccessor *mpCheckPath;
    MapdataCheckPointAccessor *mpCheckPoint;
    void *mpPointInfo;
    void *mpGeoObj;
    void *mpArea;
    void *mpCamera;
    MapdataJugemPointAccessor *mpJugemPoint;
    void *mpCannonPoint;
    void *mpStageInfo;
    void *mpMissionPoint;

    void *mGoalCamera;
    void *mType9Camera;
    void *mOpeningPanCamera;
    u32 _50; // UNK

private:
    static CourseMap *s_instance;
};
static_assert(sizeof(CourseMap) == 0x54);

} // namespace System
