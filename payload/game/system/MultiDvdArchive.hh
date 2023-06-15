#pragma once

#include "DvdArchive.hh"

namespace System {

enum class ResChannelId {
    Race = 0,
    Course = 1,
    Menu = 2,
    Font = 3,
};

class MultiDvdArchive {
public:
    enum class Format {
        Double = 0,
        Single = 1,
    };

    MultiDvdArchive(u16 archiveCount);
    virtual ~MultiDvdArchive();
    virtual void dt(s32 type);

    REPLACE virtual void init();
    void REPLACED(init)();

    void clear();
    void load(const char *path, EGG::Heap *archiveHeap, EGG::Heap *fileHeap, u32);
    void loadOther(MultiDvdArchive *other, EGG::Heap *heap);

    void setMission(u32 missionId);

    DvdArchive &archive(u16 i);
    u16 count() const;

    bool isLoaded();
    bool exists(const char *path);

    static REPLACE MultiDvdArchive *Create(ResChannelId type);

protected:
    DvdArchive *m_archives;
    u16 m_archiveCount;
    u8 _0a[0x10 - 0x0a];
    char **m_names;
    u8 _14[0x18 - 0x14];
    Format *m_formats;
};
static_assert(sizeof(MultiDvdArchive) == 0x1c);

class RaceMultiDvdArchive : public MultiDvdArchive {
public:
    RaceMultiDvdArchive();
    ~RaceMultiDvdArchive() override;
    void init() override;
};

class CourseMultiDvdArchive : public MultiDvdArchive {
public:
    CourseMultiDvdArchive();
    ~CourseMultiDvdArchive() override;
    void init() override;
};

class MenuMultiDvdArchive : public MultiDvdArchive {
public:
    MenuMultiDvdArchive();
    ~MenuMultiDvdArchive() override;
    void init() override;
};

class FontMultiDvdArchive : public MultiDvdArchive {
public:
    FontMultiDvdArchive();
    ~FontMultiDvdArchive() override;
    void init() override;
};

} // namespace System
