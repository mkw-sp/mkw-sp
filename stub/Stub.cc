#include "FS.hh"

#include "Archive.hh"
#include "LZMA.hh"

#include <common/DCache.hh>
#include <common/ICache.hh>
extern "C" {
#include <common/Paths.h>
}

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

typedef void (*LoaderEntryFunc)(void);

#pragma pack(push, 4)
struct Ticket {
    u32 signatureType;
    u8 _004[0x1dc - 0x004];
    u64 titleID;
    u16 accessMask;
    u8 _1e6[0x222 - 0x1e6];
    u8 contentAccessMask[512 / 8];
    u8 _262[0x2a4 - 0x262];
};
static_assert(sizeof(Ticket) == 0x2a4);
#pragma pack(pop)

#ifdef SP_RELEASE
#define TMP_TICKET_PATH "/tmp/524d4341.tik"
#define TICKET_PATH "/ticket/00010001/524d4341.tik"
#endif

#if defined(SP_RELEASE) || defined(SP_CHANNEL)
#define CONTENTS_PATH TITLE_DATA_PATH "/contents.arc"
#else
#define CONTENTS_PATH "/tmp/contents.arc"
#endif

// Source: https://stackoverflow.com/questions/34796571
#define ALIGNED_STRING(S)  (struct { _Alignas(16) char s[sizeof S]; }){ S }.s

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
    IOS::Init();

#ifndef SP_CHANNEL
    std::optional<Archive> embeddedArchive = LoadArchive(embeddedContents, embeddedContentsSize);
    if (!embeddedArchive) {
        return {};
    }
    std::optional<const VersionInfo *> embeddedVersionInfo = GetVersionInfo(*embeddedArchive);
    if (!embeddedVersionInfo) {
        return {};
    }
#endif

    Archive *archive;
    IOS::FS fs;
#ifdef SP_RELEASE
    fs.createDir(ALIGNED_STRING(TITLE_PATH));
    fs.createDir(ALIGNED_STRING(TITLE_DATA_PATH));
    alignas(0x20) Ticket ticket{};
    ticket.signatureType = 0x10001; // RSA-2048
    ticket.titleID = CHANNEL_TITLE_ID;
    ticket.accessMask = 0xffff;
    memset(ticket.contentAccessMask, 0xff, sizeof(ticket.contentAccessMask));
    alignas(0x20) Ticket nandTicket;
    if (!fs.readFile(ALIGNED_STRING(TICKET_PATH), &nandTicket, sizeof(nandTicket)) ||
            memcmp(&ticket, &nandTicket, sizeof(Ticket))) {
        fs.writeFile(ALIGNED_STRING(TMP_TICKET_PATH), &ticket, sizeof(ticket));
        fs.rename(ALIGNED_STRING(TMP_TICKET_PATH), ALIGNED_STRING(TICKET_PATH));
    }
#endif

#if defined(SP_RELEASE) || defined(SP_CHANNEL)
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
#ifdef SP_CHANNEL
    if (nandVersionInfo) {
#else
    if (nandVersionInfo && **nandVersionInfo >= **embeddedVersionInfo) {
#endif
        archive = &*nandArchive;
        memcpy(&versionInfo, *nandVersionInfo, sizeof(versionInfo));
    } else {
#endif
#ifdef SP_CHANNEL
        return {};
#else
        fs.writeFile(ALIGNED_STRING(CONTENTS_PATH), embeddedContents, embeddedContentsSize);
        archive = &*embeddedArchive;
        memcpy(&versionInfo, *embeddedVersionInfo, sizeof(versionInfo));
#endif
#if defined(SP_RELEASE) || defined(SP_CHANNEL)
    }
#endif

    DCache::Flush(&versionInfo);

    auto entry = archive->get("./bin/loader.bin.lzma");
    Archive::File *file = std::get_if<Archive::File>(&entry);
    if (!file) {
        return {};
    }
    u8 *loader = reinterpret_cast<u8 *>(0x80b00000);
    std::optional<size_t> loaderSize = LZMA::Decode(file->data, loader, file->size, 0xc00000);
    if (!loaderSize) {
        return {};
    }
    DCache::Flush(loader, *loaderSize);
    ICache::Invalidate(loader, *loaderSize);

    return reinterpret_cast<LoaderEntryFunc>(loader);
}

} // namespace Stub

extern "C" void Stub_run() {
    std::optional<Stub::LoaderEntryFunc> loaderEntry = Stub::Run();
    if (loaderEntry) {
        (*loaderEntry)();
    }
}
