#include "DecompLoader.hh"

#include "sp/Exchange.hh"
#include "sp/LZ77Decoder.hh"
#include "sp/LZMADecoder.hh"
#include "sp/ThumbnailManager.hh"
#include "sp/YAZDecoder.hh"
#include "sp/storage/Storage.hh"

#include <game/util/Registry.hh>

#include <common/Bytes.hh>

#include <cstring>
#include <memory>

namespace SP::Storage::DecompLoader {

struct StartInfo {
    const char *path;
    size_t maxSize;
    u64 offset;
};

static Exchange<StartInfo, Empty> startExchange;
static Exchange<bool, s32> updateExchange;
static OSThread thread;
static u8 stack[0x2000 /* 8 KiB */];
alignas(0x20) static u8 srcs[2][0x20000 /* 128 KiB */];

static std::optional<FileHandle> Open(const char *path) {
    if (ThumbnailManager::IsActive()) {
        char coursePath[128];
        snprintf(coursePath, std::size(coursePath), "ro:/Race/Course/%s.szs",
                Registry::courseFilenames[ThumbnailManager::CourseId()]);
        if (!strcmp(path, coursePath)) {
            auto thumbnailPath = ThumbnailManager::Path();
            return Storage::Open(thumbnailPath.data(), "r");
        }
    }

    size_t length = strlen(path);
    if (!strncmp(path, "ro:/", strlen("ro:/")) && length >= strlen(".szs") &&
            !strcmp(path + length - strlen(".szs"), ".szs")) {
        wchar_t lzmaPath[128];
        swprintf(lzmaPath, std::size(lzmaPath), L"%.*s.arc.lzma", length - strlen(".szs"), path);
        auto file = Storage::Open(lzmaPath, "r");
        if (file) {
            return file;
        }
    }

    wchar_t szsPath[128];
    swprintf(szsPath, std::size(szsPath), L"%s", path);
    return Storage::Open(szsPath, "r");
}

static void Read(StartInfo info) {
    auto file = Open(info.path);
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

static void *Handle(void *UNUSED(arg)) {
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
        EGG::Heap *heap) {
    startExchange.left({ path, srcMaxSize, srcOffset });

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
        EGG::Heap *heap) {
    char roPath[128];
    if (path[0] == '/') {
        snprintf(roPath, sizeof(roPath), "ro:%s", path);
    } else {
        snprintf(roPath, sizeof(roPath), "ro:/%s", path);
    }
    return Load(roPath, srcMaxSize, srcOffset, dst, dstSize, heap);
}

bool Load(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap) {
    return Load(path, SIZE_MAX, 0, dst, dstSize, heap);
}

bool LoadRO(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap) {
    return LoadRO(path, SIZE_MAX, 0, dst, dstSize, heap);
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
