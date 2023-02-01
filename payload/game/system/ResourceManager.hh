#pragma once

#include "game/host_system/Scene.hh"
#include "game/system/MultiDvdArchive.hh"

#include <egg/core/eggHeap.hh>

namespace System {

extern const char *courseFilenames[0x28];

class ResourceManager {
public:
    class CourseCache {
        friend class ResourceManager;

    public:
        enum class State {
            Cleared = 0,
            Loaded = 2,
        };

        void init();
        void deinit();

        REPLACE void load(u32 courseId);
        void load(u32 courseId, bool splitScreen);

    private:
        u8 _00[0x10 - 0x00];
        void *m_buffer;
        EGG::Heap *m_heap;
        u32 m_course;
        State m_state;
        MultiDvdArchive *m_archive;
    };
    static_assert(sizeof(CourseCache) == 0x24);

    ResourceManager();

    void createMenuHeaps(u32 count, s32 heapIdx);
    void preloadCourseAsync(u32 courseId);
    void process();

    void initGlobeHeap();
    void deinitGlobeHeap();

    DvdArchive *getMenuArchive(size_t idx);
    REPLACE u16 getMenuArchiveCount() const;
    REPLACE MultiDvdArchive *loadCourse(u32 courseId, EGG::Heap *heap, bool splitScreen);
    REPLACE MultiDvdArchive *loadMission(u32 courseId, u32 missionId, EGG::Heap *heap, bool splitScreen);

    static void ComputeCourseSHA1(u32 courseId, u8 *courseSHA1);
    static void OnCreateScene(RKSceneID sceneId);
    static REPLACE ResourceManager *CreateInstance();
    static ResourceManager *Instance();

private:
    void loadGlobe(u8 **dst);

    REPLACE static void LoadGlobeTask(void *arg);

    u8 _000[0x004 - 0x000];
    MultiDvdArchive **m_archives;
    u8 _008[0x588 - 0x008];
    CourseCache m_courseCache;
    u8 _5ac[0x60c - 0x5ac];
    bool m_globeLoadingIsBusy;
    u8 _60d[0x614 - 0x60d];
    EGG::Heap *m_globeHeap;
    u8 _618[0x61c - 0x618];
    u8 *m_globe;

    static ResourceManager *s_instance;
};
static_assert(sizeof(ResourceManager) > 0x61c);

} // namespace System
