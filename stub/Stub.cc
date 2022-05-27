#include "FS.hh"

#include "Archive.hh"

#include <common/ICache.hh>

#include <cstring>

namespace Stub {

typedef void (*LoaderEntryFunc)(void);

alignas(0x20) static const char contentsTmpPath[] = "/tmp/contents.arc";

extern "C" const u8 contents[];
extern "C" const u32 contentsSize;

static void run() {
    IOS::Init();

    {
        IOS::FS fs;
        fs.writeFile(contentsTmpPath, contents, contentsSize);
    }

    Archive archive(contents, contentsSize);
    if (!archive.ok()) {
        return;
    }
    auto entry = archive.get("./bin/loader.bin");
    Archive::File *loader = std::get_if<Archive::File>(&entry);
    if (!loader) {
        return;
    }
    memcpy(reinterpret_cast<void *>(0x80b00000), loader->data, loader->size);
    ICache::Invalidate(reinterpret_cast<void *>(0x80b00000), loader->size);

    LoaderEntryFunc loaderEntry = reinterpret_cast<LoaderEntryFunc>(0x80b00000);
    loaderEntry();
}

} // namespace Stub

extern "C" void Stub_run() {
    Stub::run();
}
