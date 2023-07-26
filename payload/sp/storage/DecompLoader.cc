#include "DecompLoader.hh"

#include "sp/Exchange.hh"
#include "sp/LZ77Decoder.hh"
#include "sp/LZMADecoder.hh"
#include "sp/ThumbnailManager.hh"
#include "sp/WBZDecoder.hh"
#include "sp/YAZDecoder.hh"

#include <game/system/RaceConfig.hh>
#include <game/system/ResourceManager.hh>

#include <common/Bytes.hh>

#include <cstring>
#include <memory>

namespace SP::Storage::DecompLoader {

struct StartInfo {
    const char *path;
    size_t maxSize;
    u64 offset;
    std::optional<StorageType> storageType;
};

static Exchange<StartInfo, Empty> startExchange;
static Exchange<bool, s32> updateExchange;
static u8 stack[0x2000 /* 8 KiB */];
static OSThread thread;
alignas(0x20) static u8 srcs[2][0x20000 /* 128 KiB */];

std::optional<FileHandle> ReadOptStorage(const wchar_t *path,
        std::optional<StorageType> storageType) {
    if (storageType.has_value()) {
        auto *storage = Storage::GetStorage(*storageType);
        return storage->open(path, "r");
    } else {
        return Storage::Open(path, "r");
    }
}

static std::optional<FileHandle> Open(const char *path, std::optional<StorageType> storageType) {
    auto *raceConfig = System::RaceConfig::Instance();

    // This is called before the game is loaded, so the nullptr check is actually needed.
    if (raceConfig != nullptr && raceConfig->m_spRace.pathReplacement.m_len != 0) {
        auto courseId = raceConfig->raceScenario().courseId;
        auto courseFilename = System::ResourceManager::GetCourseFilename(courseId);

        char coursePath[128];
        snprintf(coursePath, std::size(coursePath), "ro:/Race/Course/%s.szs", courseFilename);
        if (!strcmp(path, coursePath)) {
            // We remove the pathReplacement to prevent further unnecessary checks.
            FixedString<64> pathReplacement = raceConfig->m_spRace.pathReplacement;
            raceConfig->m_spRace.pathReplacement = "";

            // Recursive call to allow for .arc.lzma or .wbz to be added on.
            return Open(pathReplacement.c_str(), storageType);
        }
    }

    size_t length = strlen(path);
    std::string_view pathSv(path, length);
    if (pathSv.starts_with("ro:/") && pathSv.ends_with(".szs")) {
        wchar_t testPath[128];
        std::optional<FileHandle> file;

        swprintf(testPath, std::size(testPath), L"%.*s.arc.lzma", length - strlen(".szs"), path);
        if ((file = ReadOptStorage(testPath, storageType))) {
            return file;
        }

        swprintf(testPath, std::size(testPath), L"%.*s.wbz", length - strlen(".szs"), path);
        if ((file = ReadOptStorage(testPath, storageType))) {
            return file;
        }
    }

    wchar_t szsPath[128];
    swprintf(szsPath, std::size(szsPath), L"%s", path);
    return ReadOptStorage(szsPath, storageType);
}

static void Read(StartInfo info) {
    auto file = Open(info.path, info.storageType);
    if (!file || info.offset > file->size()) {
        updateExchange.right(-1);
        return;
    }

    u64 size = std::min(file->size(), info.offset + info.maxSize);
    for (u32 offset = info.offset, i = 0; offset < size; offset += sizeof(*srcs), i = (i + 1) % 2) {
        s32 srcSize = MIN(size - offset, sizeof(*srcs));
        if (!file->read(srcs[i], srcSize, offset)) {
            updateExchange.right(-1);
            return;
        }

        if (!updateExchange.right(srcSize)) {
            return;
        }
    }

    updateExchange.right(0);
}

static void *Handle(void * /* arg */) {
    while (true) {
        auto info = startExchange.right({});
        Read(info);
    }
}

void Init() {
    OSCreateThread(&thread, Handle, nullptr, stack + sizeof(stack), sizeof(stack), 24, 0);
    OSResumeThread(&thread);
}

bool Load(const char *path, size_t srcMaxSize, u64 srcOffset, u8 **dst, size_t *dstSize,
        EGG::Heap *heap, std::optional<StorageType> storageType) {
    startExchange.left({path, srcMaxSize, srcOffset, storageType});

    u32 i = 0;
    const u8 *src = srcs[i];
    s32 srcSize = updateExchange.left(true);
    if (srcSize <= 0) {
        return false;
    } else if (static_cast<size_t>(srcSize) < sizeof(u32)) {
        updateExchange.left(false);
        return false;
    }

    std::unique_ptr<Decoder> decoder;
    if (YAZDecoder::CheckMagic(Bytes::Read<u32>(src, 0x0))) {
        decoder.reset(new (heap, 0x4) YAZDecoder(src, srcSize, heap));
    } else if (LZ77Decoder::CheckMagic(Bytes::Read<u32, std::endian::little>(src, 0x0))) {
        decoder.reset(new (heap, 0x4) LZ77Decoder(src, srcSize, heap));
    } else if (WBZDecoder::CheckMagic(Bytes::Read<u64, std::endian::big>(src, 0x0))) {
        decoder.reset(new (heap, 0x4) WBZDecoder(src, srcSize, heap));
    } else {
        decoder.reset(new (heap, 0x4) LZMADecoder(src, srcSize, heap));
    }

    src += decoder->headerSize();
    srcSize -= decoder->headerSize();

    while (decoder->ok() && !decoder->done() && decoder->decode(src, srcSize)) {
        srcSize = updateExchange.left(true);
        if (srcSize < 0 || (srcSize == 0 && !decoder->done())) {
            return false;
        } else if (srcSize == 0 && decoder->done()) {
            decoder->release(dst, dstSize);
            return true;
        }

        i = (i + 1) % 2;
        src = srcs[i];
    }

    updateExchange.left(false);
    return false;
}

bool LoadRO(const char *path, size_t srcMaxSize, u64 srcOffset, u8 **dst, size_t *dstSize,
        EGG::Heap *heap, std::optional<StorageType> storageType) {
    char roPath[128];
    if (path[0] == '/') {
        snprintf(roPath, sizeof(roPath), "ro:%s", path);
    } else {
        snprintf(roPath, sizeof(roPath), "ro:/%s", path);
    }
    return Load(roPath, srcMaxSize, srcOffset, dst, dstSize, heap, storageType);
}

bool Load(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap,
        std::optional<StorageType> storageType) {
    return Load(path, SIZE_MAX, 0, dst, dstSize, heap, storageType);
}

bool LoadRO(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap,
        std::optional<StorageType> storageType) {
    return LoadRO(path, SIZE_MAX, 0, dst, dstSize, heap, storageType);
}

} // namespace SP::Storage::DecompLoader

extern "C" bool DecompLoader_Load(const char *path, u8 **dst, size_t *dstSize, EGG_Heap *heap) {
    auto *eggHeap = reinterpret_cast<EGG::Heap *>(heap);
    return SP::Storage::DecompLoader::Load(path, dst, dstSize, eggHeap);
}

extern "C" bool DecompLoader_LoadRO(const char *path, u8 **dst, size_t *dstSize, EGG_Heap *heap) {
    auto *eggHeap = reinterpret_cast<EGG::Heap *>(heap);
    return SP::Storage::DecompLoader::LoadRO(path, dst, dstSize, eggHeap);
}
