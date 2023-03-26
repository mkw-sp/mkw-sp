#include "ResourceManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RootScene.hh"

#include <game/util/Registry.hh>
#include <sp/TrackPackManager.hh>
#include <sp/storage/DecompLoader.hh>

extern "C" {
#include <revolution.h>
}

#include <cstdio>
#include <cstring>

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

void ResourceManager::ComputeCourseSHA1(u8 *courseSHA1) {
    NETSHA1Context cx;
    NETSHA1Init(&cx);
    DvdArchive &archive =
            s_instance->m_archives[static_cast<size_t>(MultiDvdArchive::Type::Course)]->archive(0);
    NETSHA1Update(&cx, archive.buffer(), static_cast<u32>(archive.size()));
    NETSHA1GetDigest(&cx, courseSHA1);
}

void ResourceManager::OnCreateScene(RKSceneID sceneId) {
    switch (sceneId) {
    case RKSceneID::Menu:
    case RKSceneID::Race:
        s_instance->deinitGlobeHeap();
        break;
    case RKSceneID::Globe:
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

MultiDvdArchive *ResourceManager::loadCourse(u32 courseId, EGG::Heap *heap, bool splitScreen) {
    SP_LOG("Loading course %d", courseId);

    MultiDvdArchive *archive = m_archives[1];
    if (archive->isLoaded()) {
        return archive;
    };

    archive->init();

    char filePath[64];
    auto trackPackManager = SP::TrackPackManager::Instance();

    if (splitScreen) {
        trackPackManager->getTrackPath(filePath, sizeof(filePath), courseId, true);
        if (!archive->exists(filePath)) {
            splitScreen = false;
        }
    }

    if (!splitScreen) {
        trackPackManager->getTrackPath(filePath, sizeof(filePath), courseId, false);
    }

    // Setup stock game slots
    auto slotId = trackPackManager->getSelectedTrackPack()->getSlotId(courseId);
    RaceConfig::Instance()->raceScenario().courseId = slotId;

    JobContext *jobContext = &m_jobContexts[2];
    jobContext->multiArchive = archive;
    jobContext->archiveHeap = heap;
    strncpy(jobContext->filename, filePath, sizeof(jobContext->filename));

    m_taskThread->request(&ResourceManager_doLoadTask, 2, 0);
    process();

    if (!archive->isLoaded()) {
        OSSleepMilliseconds(10000);
    }

    assert(archive->isLoaded());
    return archive;
}

void ResourceManager::CourseCache::init() {}

void ResourceManager::CourseCache::load(u32 /* courseId */) {}

} // namespace System
