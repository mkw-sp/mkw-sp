#include "ResourceManager.hh"

#include <sp/storage/DecompLoader.hh>
#include <sp/storage/Storage.hh>

extern "C" {
#include <revolution.h>
}

namespace System {

ResourceManager *ResourceManager::CreateInstance() {
    s_instance = new ResourceManager;
    assert(s_instance);

    s_instance->m_globe = nullptr;

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

} // namespace System
