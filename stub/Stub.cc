#include "FS.hh"

#include "Archive.hh"
#include "LZMA.hh"

#include <common/DCache.hh>
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

    auto entry = archive.get("./bin/version.bin");
    Archive::File *file = std::get_if<Archive::File>(&entry);
    if (!file) {
        return;
    }
    memcpy(&versionInfo, file->data, file->size);
    DCache::Flush(file->data, file->size);

    entry = archive.get("./bin/loader.bin.lzma");
    file = std::get_if<Archive::File>(&entry);
    if (!file) {
        return;
    }
    u8 *loader = reinterpret_cast<u8 *>(0x80b00000);
    std::optional<size_t> loaderSize = LZMA::Decode(file->data, loader, file->size, 0xc00000);
    if (!loaderSize) {
        return;
    }
    DCache::Flush(loader, *loaderSize);
    ICache::Invalidate(loader, *loaderSize);

    LoaderEntryFunc loaderEntry = reinterpret_cast<LoaderEntryFunc>(loader);
    loaderEntry();
}

} // namespace Stub

extern "C" void Stub_run() {
    Stub::run();
}
