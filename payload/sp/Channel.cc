#include "Channel.hh"

#include "sp/storage/DecompLoader.hh"

#include <common/ES.hh>
#include <common/Paths.hh>
extern "C" {
#include <revolution.h>
}

#include <array>
#include <cstdio>
#include <cstring>
#include <optional>

namespace SP::Channel {

struct ContentFile {
    u8 *data = nullptr;
    size_t size = 0;
};

#define TMP_CONTENT_PATH "/tmp/content"

static const std::array<const char *, 2> contentPaths{"opening.bnr.lzma", "boot.dol.lzma"};
static std::array<ContentFile, 2> contentFiles;
static Status status = Status::None;
static bool isLoaded = false;

static_assert(contentPaths.size() + 1 == CHANNEL_CONTENT_COUNT);
static_assert(contentFiles.size() + 1 == CHANNEL_CONTENT_COUNT);

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
    } else if (count != contentFiles.size() + 1) {
        SP_LOG("Mismatched content count");
        return;
    }

    if (tmdView.titleVersion < CHANNEL_TITLE_VERSION) {
        status = Status::Older;
    } else if (tmdView.titleVersion == CHANNEL_TITLE_VERSION) {
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
        ContentFile &file = contentFiles[i];
        assert(Storage::DecompLoader::LoadRO(path, &file.data, &file.size, heap));
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

    for (size_t i = 0; i < contentFiles.size(); i++) {
        char path[NAND_MAX_PATH];
        snprintf(path, sizeof(path), TMP_CONTENT_PATH "/%08x.app", i);
        if (!WriteFile(path, contentFiles[i].data, contentFiles[i].size)) {
            SP_LOG("Failed to write %08x.app", i);
            return;
        }
    }

    NANDFileInfo fileInfo;
    if (NANDPrivateOpen(TMP_NAND_LOADER_PATH, &fileInfo, NAND_ACCESS_READ) != NAND_RESULT_OK) {
        SP_LOG("Failed to open NAND loader.");
        return;
    }
    u32 nandLoaderSize;
    if (NANDGetLength(&fileInfo, &nandLoaderSize) != NAND_RESULT_OK) {
        SP_LOG("Failed to get NAND loader size.");
        NANDClose(&fileInfo);
        return;
    }
    if (NANDClose(&fileInfo) != NAND_RESULT_OK) {
        SP_LOG("Failed to close NAND loader.");
        return;
    }
    if (NANDPrivateMove(TMP_NAND_LOADER_PATH, TMP_CONTENT_PATH) != NAND_RESULT_OK) {
        SP_LOG("Failed to move NAND loader.");
        return;
    }

    alignas(0x20) IOS::ES::Tmd tmd{};
    tmd.signatureType = 0x10001; // RSA-2048
    tmd.iosID = UINT64_C(0x000000010000003a);
    tmd.titleID = CHANNEL_TITLE_ID;
    tmd.titleType = 0;
    tmd.groupID = 0x3031;
    tmd.region = 3;  // Region free
    tmd._1ae[4] = 1; // Skip drive reset
    tmd.titleVersion = CHANNEL_TITLE_VERSION;
    tmd.numContents = contentFiles.size() + 1;
    tmd.bootIndex = 2;
    for (size_t i = 0; i < contentFiles.size(); i++) {
        tmd.contents[i].id = i;
        tmd.contents[i].index = i;
        tmd.contents[i].type = 1;
        tmd.contents[i].size = contentFiles[i].size;
    }
    tmd.contents[2].id = 2;
    tmd.contents[2].index = 2;
    tmd.contents[2].type = 1;
    tmd.contents[2].size = nandLoaderSize;

    u32 size = offsetof(IOS::ES::Tmd, contents) + sizeof(*tmd.contents) * tmd.numContents;
    if (!WriteFile(TMP_CONTENT_PATH "/title.tmd", &tmd, size)) {
        SP_LOG("Failed to write title.tmd");
        return;
    }

    if (NANDPrivateMove(TMP_CONTENT_PATH, TITLE_PATH) != NAND_RESULT_OK) {
        SP_LOG("Failed to copy content directory");
    }
}

void Install() {
    InstallInternal();
    Check();
}

} // namespace SP::Channel

extern "C" void Channel_Init() {
    SP::Channel::Init();
}
