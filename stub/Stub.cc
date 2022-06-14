#include "FS.hh"

#include "Archive.hh"
#include "LZMA.hh"

#include <common/Console.hh>
#include <common/DCache.hh>
#include <common/ES.hh>
#include <common/ICache.hh>
extern "C" {
#include <common/Paths.h>
}
#include <common/VI.hh>

#include <compare>
#include <cstring>

auto operator<=>(const VersionInfo& lhs, const VersionInfo &rhs) {
    if (auto cmp = lhs.major <=> rhs.major; cmp != 0) {
        return cmp;
    }
    if (auto cmp = lhs.minor <=> rhs.minor; cmp != 0) {
        return cmp;
    }
    if (auto cmp = lhs.patch <=> rhs.patch; cmp != 0) {
        return cmp;
    }
    return std::strong_ordering::equal;
}

namespace Stub {

extern "C" volatile u32 aicr;

typedef void (*LoaderEntryFunc)(void);

#ifdef SP_RELEASE
#define TMP_TICKET_PATH "/tmp/524b5350.tik"
#define TICKET_PATH "/ticket/00010001/524b5350.tik"
#define TMD_PATH TITLE_CONTENT_PATH "/title.tmd"
#define BANNER_PATH TITLE_DATA_PATH "/banner.bin"
#endif

#if defined(SP_RELEASE) || defined(SP_CHANNEL)
#define CONTENTS_PATH TITLE_DATA_PATH "/contents.arc"
#else
#define CONTENTS_PATH "/tmp/contents.arc"
#endif

#ifndef SP_CHANNEL
extern "C" const u8 embeddedContents[];
extern "C" const u32 embeddedContentsSize;
#endif

static std::optional<Archive> LoadArchive(const u8 *data, u32 size) {
    Archive archive(data, size);
    if (!archive.ok()) {
        return {};
    }
    return archive;
}

static std::optional<const VersionInfo *> GetVersionInfo(Archive &archive) {
    auto entry = archive.get("./bin/version.bin");
    Archive::File *file = std::get_if<Archive::File>(&entry);
    if (!file || file->size != sizeof(VersionInfo)) {
        return {};
    }
    return reinterpret_cast<const VersionInfo *>(file->data);
}

static std::optional<LoaderEntryFunc> Run() {
    // Reset the DSP: libogc apps like the HBC cannot initialize it properly, but the SDK can.
    aicr = 0;

    VI::Init();

    Console::Init();
    Console::Print("MKW-SP Stub\n");

    Console::Print("Initializing IOS...");
    IOS::Init();
    Console::Print(" done.\n");

#ifndef SP_CHANNEL
    Console::Print("Loading the embedded archive...");
    std::optional<Archive> embeddedArchive = LoadArchive(embeddedContents, embeddedContentsSize);
    if (!embeddedArchive) {
        Console::Print(" failed!\n");
        return {};
    }
    Console::Print(" done.\n");
    std::optional<const VersionInfo *> embeddedVersionInfo = GetVersionInfo(*embeddedArchive);
    if (!embeddedVersionInfo) {
        return {};
    }
#endif

    Archive *archive;
    Console::Print("Escalating privileges...");
    if (!IOS::EscalatePrivileges()) {
        Console::Print(" failed!\n");
        return {};
    }
    Console::Print(" done.\n");
    Console::Print("Initializing FS...");
    IOS::FS fs;
    Console::Print(" done.\n");
    Console::Print("Deescalating privileges...");
    IOS::DeescalatePrivileges();
    Console::Print(" done.\n");
    if (!fs.ok()) {
        return {};
    }
#ifdef SP_RELEASE
    Console::Print("Creating directories...");
    fs.createDir(ALIGNED_STRING(TITLE_PATH));
    fs.createDir(ALIGNED_STRING(TITLE_CONTENT_PATH));
    fs.createDir(ALIGNED_STRING(TITLE_DATA_PATH));
    fs.createDir(ALIGNED_STRING(UPDATE_PATH));
    Console::Print(" done.\n");
#endif

#if defined(SP_RELEASE) || defined(SP_CHANNEL)
    fs.rename(UPDATE_CONTENTS_PATH, CONTENTS_PATH);

    Console::Print("Loading the NAND archive...");
    std::optional<Archive> nandArchive{};
    std::optional<const VersionInfo *> nandVersionInfo{};

    u8 *nandContents = reinterpret_cast<u8 *>(0x90000000);
    auto nandContentsSize = fs.readFile(ALIGNED_STRING(CONTENTS_PATH), nandContents, 0x3000000);

    if (nandContentsSize) {
        nandArchive = LoadArchive(nandContents, *nandContentsSize);
    }
    if (nandArchive) {
        nandVersionInfo = GetVersionInfo(*nandArchive);
    }
    if (nandVersionInfo) {
        Console::Print(" done.\n");
    } else {
        Console::Print(" failed!\n");
    }
#ifdef SP_CHANNEL
    if (nandVersionInfo) {
#else
    if (nandVersionInfo && **nandVersionInfo >= **embeddedVersionInfo) {
#endif
        Console::Print("Using the NAND archive.\n");
        archive = &*nandArchive;
        memcpy(&versionInfo, *nandVersionInfo, sizeof(versionInfo));
    } else {
#else
    {
#endif
#ifdef SP_CHANNEL
        return {};
#else
        Console::Print("Using the embedded archive.\n");
        archive = &*embeddedArchive;
        memcpy(&versionInfo, *embeddedVersionInfo, sizeof(versionInfo));

#ifdef SP_RELEASE
        alignas(0x20) IOS::ES::Ticket ticket{};
        ticket.signatureType = 0x10001; // RSA-2048
        ticket.titleID = CHANNEL_TITLE_ID;
        ticket.accessMask = 0xffff;
        memset(ticket.contentAccessMask, 0xff, sizeof(ticket.contentAccessMask));
        if (!fs.writeFile(ALIGNED_STRING(TICKET_PATH), &ticket, sizeof(ticket))) {
            Console::Print("Failed to write the ticket to the NAND!\n");
        }

        if (!IOS::File(ALIGNED_STRING(TMD_PATH), IOS::Mode::Read).ok()) {
            alignas(0x20) IOS::ES::Tmd tmd{};
            tmd.signatureType = 0x10001; // RSA-2048
            tmd.iosID = UINT64_C(0x000000010000003a);
            tmd.titleID = CHANNEL_TITLE_ID;
            tmd.titleType = 1;
            tmd.groupID = 0x3031;
            tmd.region = 3; // Region free
            memset(tmd.ratings, 0x80, sizeof(tmd.ratings));
            tmd._1ae[4] = 1; // Skip drive reset
            tmd.titleVersion = CHANNEL_TITLE_VERSION;
            tmd.numContents = CHANNEL_CONTENT_COUNT;
            tmd.bootIndex = 1;
            for (size_t i = 0; i < CHANNEL_CONTENT_COUNT; i++) {
                tmd.contents[i].id = i;
                tmd.contents[i].index = i;
                tmd.contents[i].type = 1;
                tmd.contents[i].size = 0;
            }
            u32 size = offsetof(IOS::ES::Tmd, contents) + sizeof(*tmd.contents) * tmd.numContents;
            if (!fs.writeFile(ALIGNED_STRING(TMD_PATH), &ticket, size)) {
                Console::Print("Failed to write the tmd to the NAND!\n");
            }
        }

        auto entry = archive->get("./banner.bin");
        Archive::File *file = std::get_if<Archive::File>(&entry);
        if (!file) {
            Console::Print("Failed to find the save banner!\n");
            return {};
        }
        if (!fs.writeFile(ALIGNED_STRING(BANNER_PATH), file->data, file->size)) {
            Console::Print("Failed to copy the save banner to the NAND!\n");
        }
#endif

        if (!fs.writeFile(ALIGNED_STRING(CONTENTS_PATH), embeddedContents, embeddedContentsSize)) {
            Console::Print("Failed to copy the embedded archive to the NAND!\n");
        }
#endif
    }

    DCache::Flush(versionInfo);

    Console::Print("Opening the loader...");
    auto entry = archive->get("./bin/loader.bin.lzma");
    Archive::File *file = std::get_if<Archive::File>(&entry);
    if (!file) {
        Console::Print(" failed!\n");
        return {};
    }
    Console::Print(" done.\n");
    Console::Print("Decompressing the loader...");
    u8 *loader = reinterpret_cast<u8 *>(0x80b00000);
    std::optional<size_t> loaderSize = LZMA::Decode(file->data, loader, file->size, 0xb00000);
    if (!loaderSize) {
        Console::Print(" failed!\n");
        return {};
    }
    Console::Print(" done.\n");
    DCache::Flush(loader, *loaderSize);
    ICache::Invalidate(loader, *loaderSize);

    Console::Print("Running the loader...\n");
    return reinterpret_cast<LoaderEntryFunc>(loader);
}

} // namespace Stub

extern "C" void Stub_Run() {
    // On console, bad stuff seems to happen when writing to the XFB, presumably when some cache
    // lines are written back to main memory. Prevent that by completely emptying the dcache.
    DCache::Invalidate(reinterpret_cast<void *>(0x80000000), 0x1800000);

    std::optional<Stub::LoaderEntryFunc> loaderEntry = Stub::Run();
    if (loaderEntry) {
        (*loaderEntry)();
    }
}
