#include "Channel.hh"

#include "sp/DVDDecompLoader.hh"

extern "C" {
#include <common/Paths.h>

#include <revolution.h>
}

#include <array>
#include <cstdio>
#include <cstring>

extern "C" const u8 opening[];
extern "C" const u32 openingSize;
extern "C" const u8 channel[];
extern "C" const u32 channelSize;

namespace SP::Channel {

#pragma pack(push, 4)
struct Content {
    u32 id;
    u16 index;
    u16 type;
    u64 size;
    u8 sha1[0x14];
};
static_assert(sizeof(Content) == 0x24);

struct Tmd {
    u32 signatureType;
    u8 _004[0x184 - 0x004];
    u64 iosID;
    u64 titleID;
    u32 titleType;
    u16 groupID;
    u8 _19a[0x19c - 0x19a];
    u16 region;
    u8 ratings[16];
    u8 _1ae[0x1dc - 0x1ae];
    u16 titleVersion;
    u16 numContents;
    u16 bootIndex;
    Content contents[512];
};
static_assert(sizeof(Tmd) == 0x49e4);
#pragma pack(pop)

struct ContentFile {
    u8 *data = nullptr;
    size_t size = 0;
};

#define TMP_CONTENT_PATH "/tmp/content"

static const u16 titleVersion = 0x0100;
static const std::array<const char *, 2> contentPaths{ "opening.bnr.lzma", "boot.dol.lzma" };
static std::array<ContentFile, 2> contentFiles;
static Status status = Status::None;
static bool isLoaded = false;

static void Check() {
    ESP_InitLib();

    status = Status::None;

    u32 count;
    if (ESP_GetTicketViews(CHANNEL_TITLE_ID, nullptr, &count) < 0) {
        SP_LOG("Failed to get ticket count");
        return;
    } else if (count == 0) {
        SP_LOG("No ticket found");
        return;
    }

    u32 size;
    if (ESP_GetTmdView(CHANNEL_TITLE_ID, nullptr, &size) < 0) {
        SP_LOG("Failed to get tmd view size");
        return;
    } else if (size > sizeof(TmdView)) {
        SP_LOG("Invalid tmd view size");
        return;
    }

    alignas(0x20) TmdView tmdView;
    if (ESP_GetTmdView(CHANNEL_TITLE_ID, &tmdView, &size) < 0) {
        SP_LOG("Failed to get tmd view");
        return;
    }

    if (ESP_ListTitleContentsOnCard(CHANNEL_TITLE_ID, nullptr, &count) < 0) {
        SP_LOG("Failed to get content count");
        return;
    } else if (count != contentFiles.size()) {
        SP_LOG("Mismatched content count");
        return;
    }

    if (tmdView.titleVersion < titleVersion) {
        status = Status::Older;
    } else if (tmdView.titleVersion == titleVersion) {
        status = Status::Same;
    } else {
        status = Status::Newer;
    }
}

void Init() {
    Check();
}

Status GetStatus() {
    return status;
}

void Load(EGG::Heap *heap) {
    assert(!isLoaded);

    for (size_t i = 0; i < contentFiles.size(); i++) {
        char path[128];
        snprintf(path, sizeof(path), "/channel/%s", contentPaths[i]);
        assert(DVDDecompLoader::load(path, &contentFiles[i].data, &contentFiles[i].size, heap));
    }

    isLoaded = true;
}

void Unload() {
    assert(isLoaded);

    for (size_t i = 0; i < contentFiles.size(); i++) {
        delete[] contentFiles[i].data;
        contentFiles[i].data = nullptr;
        contentFiles[i].size = 0;
    }

    isLoaded = false;
}

static bool WriteFile(const char *path, const void *src, u32 size) {
    u8 perms = NAND_PERM_OWNER_MASK | NAND_PERM_GROUP_MASK | NAND_PERM_OTHER_MASK;
    if (NANDPrivateCreate(path, perms, 0) != NAND_RESULT_OK) {
        return false;
    }
    NANDFileInfo fileInfo;
    if (NANDPrivateOpen(path, &fileInfo, NAND_ACCESS_WRITE) != NAND_RESULT_OK) {
        return false;
    }
    s32 result = NANDWrite(&fileInfo, src, size);
    if (result < 0 || static_cast<u32>(result) != size) {
        NANDClose(&fileInfo);
        return false;
    }
    return NANDClose(&fileInfo) == NAND_RESULT_OK;
}

static void InstallInternal() {
    assert(isLoaded);

    NANDPrivateDelete(TMP_CONTENT_PATH);
    u8 perms = NAND_PERM_OWNER_MASK | NAND_PERM_GROUP_MASK | NAND_PERM_OTHER_MASK;
    if (NANDPrivateCreateDir(TMP_CONTENT_PATH, perms, 0) != NAND_RESULT_OK) {
        SP_LOG("Failed to create content directory");
        return;
    }

    alignas(0x20) Tmd tmd{};
    tmd.signatureType = 0x10001; // RSA-2048
    tmd.iosID = UINT64_C(0x000000010000003a);
    tmd.titleID = CHANNEL_TITLE_ID;
    tmd.titleType = 1;
    tmd.groupID = 0x3031;
    tmd.region = 3; // Region free
    memset(tmd.ratings, 0x80, sizeof(tmd.ratings));
    tmd._1ae[4] = 1; // Skip drive reset
    tmd.titleVersion = titleVersion;
    tmd.numContents = contentFiles.size();
    tmd.bootIndex = 1;
    for (size_t i = 0; i < contentFiles.size(); i++) {
        tmd.contents[i].id = i;
        tmd.contents[i].index = i;
        tmd.contents[i].type = 1;
        tmd.contents[i].size = contentFiles[i].size;
        NETSHA1Context ctx;
        NETSHA1Init(&ctx);
        NETSHA1Update(&ctx, contentFiles[i].data, contentFiles[i].size);
        NETSHA1GetDigest(&ctx, tmd.contents[i].sha1);
    }
    u32 size = offsetof(Tmd, contents) + sizeof(Content) * tmd.numContents;
    if (!WriteFile(TMP_CONTENT_PATH "/title.tmd", &tmd, size)) {
        SP_LOG("Failed to write title.tmd");
        return;
    }

    for (size_t i = 0; i < contentFiles.size(); i++) {
        char path[NAND_MAX_PATH];
        snprintf(path, sizeof(path), TMP_CONTENT_PATH "/%08x.app", i);
        if (!WriteFile(path, contentFiles[i].data, contentFiles[i].size)) {
            SP_LOG("Failed to write %08x.app", i);
            return;
        }
    }

    if (NANDPrivateMove(TMP_CONTENT_PATH, TITLE_PATH) != NAND_RESULT_OK) {
        SP_LOG("Failed to copy content directory");
    }
}

static void UninstallInternal() {
    NANDPrivateDelete(TITLE_CONTENT_PATH);
}

void Install() {
    InstallInternal();
    Check();
}

void Uninstall() {
    UninstallInternal();
    Check();
}

} // namespace SP::Channel

extern "C" void Channel_Init() {
    SP::Channel::Init();
}
