#include "FS.hh"

#include <common/ICache.hh>

#include <cstring>

namespace Stub {

typedef void (*LoaderEntryFunc)(void);

alignas(0x20) static const char assetsTmpPath[] = "/tmp/assets.arc";

extern "C" const u8 loader[];
extern "C" const u32 loaderSize;
extern "C" const u8 assets[];
extern "C" const u32 assetsSize;

static void run() {
    IOS::Init();

    {
        IOS::FS fs;
        fs.writeFile(assetsTmpPath, assets, assetsSize);
    }

    memcpy(reinterpret_cast<void *>(0x80b00000), loader, loaderSize);
    ICache::Invalidate(reinterpret_cast<void *>(0x80b00000), loaderSize);

    LoaderEntryFunc loaderEntry = reinterpret_cast<LoaderEntryFunc>(0x80b00000);
    loaderEntry();
}

} // namespace Stub

extern "C" void Stub_run() {
    Stub::run();
}
