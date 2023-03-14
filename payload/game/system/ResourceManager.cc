#include "ResourceManager.hh"

#include "game/system/RootScene.hh"

#include <sp/storage/DecompLoader.hh>
#include <sp/storage/Storage.hh>

extern "C" {
#include <revolution.h>
}

#include <cstdio>

namespace System {

void ResourceManager::initGlobeHeap() {
    if (!m_globeHeap) {
        auto *heap = RootScene::Instance()->m_heapCollection.mem2;
        size_t size = OSRoundUp32B(0x107d080 + 0x20400);
        void *buffer = new (heap, -0x20) u8[size];
        m_globeHeap = EGG::ExpHeap::Create(buffer, size, 1);
    }
}

void ResourceManager::deinitGlobeHeap() {
    if (m_globeHeap) {
        m_globe = nullptr;
        m_globeHeap->destroy();
        m_globeHeap = nullptr;
        m_globeLoadingIsBusy = false;
    }
}

DvdArchive *ResourceManager::getMenuArchive(size_t idx) {
    MultiDvdArchive *archive = m_archives[static_cast<size_t>(MultiDvdArchive::Type::Menu)];
    u16 loadedCount = 0;
    for (u16 i = 0; i < archive->count(); i++) {
        DvdArchive &candidate = archive->archive(i);
        if (candidate.state() == DvdArchive::State::Mounted) {
            if (loadedCount == idx) {
                return &candidate;
            }
            loadedCount++;
        }
    }
    return nullptr;
}

u16 ResourceManager::getMenuArchiveCount() const {
    MultiDvdArchive *archive = m_archives[static_cast<size_t>(MultiDvdArchive::Type::Menu)];
    u16 loadedCount = 0;
    for (u16 i = 0; i < archive->count(); i++) {
        if (archive->archive(i).state() == DvdArchive::State::Mounted) {
            loadedCount++;
        }
    }
    return loadedCount;
}

MultiDvdArchive *ResourceManager::loadCourse(u32 courseId, EGG::Heap *heap, bool splitScreen) {
    if (m_courseCache.m_state != CourseCache::State::Loaded || m_courseCache.m_course != courseId) {
        m_courseCache.load(courseId, splitScreen);
    }

    MultiDvdArchive *archive = m_archives[static_cast<size_t>(MultiDvdArchive::Type::Course)];
    archive->loadOther(m_courseCache.m_archive, heap);
    m_courseCache.m_state = CourseCache::State::Cleared;
    return archive;
}

MultiDvdArchive *ResourceManager::loadMission(u32 courseId, u32 missionId, EGG::Heap *heap,
        bool splitScreen) {
    MultiDvdArchive *archive = m_archives[static_cast<size_t>(MultiDvdArchive::Type::Course)];
    archive->setMission(missionId);
    m_courseCache.load(courseId, splitScreen);
    archive->loadOther(m_courseCache.m_archive, heap);
    archive->load("", heap, heap, 0);
    m_courseCache.m_state = CourseCache::State::Cleared;
    return archive;
}

void ResourceManager::ComputeCourseSHA1(u32 courseId, u8 *courseSHA1) {
    s_instance->preloadCourseAsync(courseId);
    s_instance->process();

    NETSHA1Context cx;
    NETSHA1Init(&cx);
    const DvdArchive &archive = s_instance->m_courseCache.m_archive->archive(0);
    NETSHA1Update(&cx, archive.buffer(), static_cast<u32>(archive.size()));
    NETSHA1GetDigest(&cx, courseSHA1);
}

void ResourceManager::OnCreateScene(RKSceneID sceneId) {
    switch (sceneId) {
    case RKSceneID::Menu:
    case RKSceneID::Race:
        s_instance->deinitGlobeHeap();
        s_instance->m_courseCache.init();
        break;
    case RKSceneID::Globe:
        s_instance->m_courseCache.deinit();
        s_instance->initGlobeHeap();
        break;
    default:
        break;
    }
}

ResourceManager *ResourceManager::CreateInstance() {
    s_instance = new ResourceManager;
    assert(s_instance);

    s_instance->m_globe = nullptr;

    return s_instance;
}

ResourceManager *ResourceManager::Instance() {
    return s_instance;
}

void ResourceManager::loadGlobe(u8 **dst) {
    if (m_globe) {
        *dst = m_globe;

        m_globeLoadingIsBusy = false;
        return;
    }

    const char *path = "/contents/globe.arc";
    auto *heap = m_globeHeap;

    auto file = SP::Storage::OpenRO(path);
    assert(file);

    alignas(0x20) ARCHeader header;
    assert(file->read(&header, sizeof(header), 0));

    void *metadata = heap->alloc(header.fileStart, 0x20);
    assert(metadata);
    assert(file->read(metadata, header.fileStart, 0));

    ARCHandle handle;
    assert(ARCInitHandle(metadata, &handle));

    ARCFileInfo info;
    assert(ARCOpen(&handle, "/earth.brres.LZ", &info));
    u32 srcSize = AlignUp<u32>(ARCGetLength(&info), 0x20);
    u32 srcOffset = ARCGetStartOffset(&info);
    assert(ARCClose(&info));

    size_t dstSize;
    assert(SP::Storage::DecompLoader::LoadRO(path, srcSize, srcOffset, dst, &dstSize, heap));
    m_globe = *dst;

    m_globeLoadingIsBusy = false;
}

void ResourceManager::LoadGlobeTask(void *arg) {
    assert(s_instance);
    s_instance->loadGlobe(reinterpret_cast<u8 **>(arg));
}

void ResourceManager::CourseCache::init() {
    if (!m_heap) {
        assert(!m_buffer);
        auto *heap = RootScene::Instance()->m_heapCollection.mem2;
        m_buffer = new (heap, -0x20) u8[0xb00000];
        m_heap = EGG::ExpHeap::Create(m_buffer, 0xb00000, 1);
        m_state = State::Cleared;
    }
}

void ResourceManager::CourseCache::deinit() {
    if (m_heap) {
        if (m_state == State::Cleared) {
            m_archive->clear();
        }
        assert(m_buffer);
        m_state = State::Cleared;
        m_heap->destroy();
        m_heap = nullptr;
        m_buffer = nullptr;
    }
}

void ResourceManager::CourseCache::load(u32 courseId) {
    load(courseId, false);
}

void ResourceManager::CourseCache::load(u32 courseId, bool splitScreen) {
    m_course = courseId;
    if (m_state == State::Loaded) {
        m_archive->clear();
    }

    char path[128];
    if (splitScreen) {
        snprintf(path, sizeof(path), "Race/Course/%s_d", courseFilenames[courseId]);
    } else {
        snprintf(path, sizeof(path), "Race/Course/%s", courseFilenames[courseId]);
    }
    SP_LOG("Loading track '%s'...", path);
    m_archive->load(path, m_heap, m_heap, 0);
    SP_LOG("Track '%s' was successfully loaded.", path);
    m_state = State::Loaded;
}

} // namespace System
