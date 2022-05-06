extern "C" {
#include "DVDDecompLoader.h"
}

#include "sp/Bytes.hh"
#include "sp/DVDFile.hh"
#include "sp/Exchange.hh"
#include "sp/LZMADecoder.hh"
#include "sp/YAZDecoder.hh"

#include <memory>

namespace SP::DVDDecompLoader {

static Exchange<const char *, Empty> startExchange;
static Exchange<bool, s32> updateExchange;
static OSThread thread;
alignas(0x8) static u8 stack[0x2000 /* 8 KiB */];
alignas(0x20) static u8 srcs[2][0x4000 /* 16 KiB */];

static void read(const char *path) {
    DVDFile file(path);
    if (!file.ok()) {
        updateExchange.right(-1);
        return;
    }

    for (u32 offset = 0x0, i = 0; offset < file.size(); offset += sizeof(*srcs), i = (i + 1) % 2) {
        s32 srcSize = MIN(file.size() - offset, sizeof(*srcs));
        u32 readSize = OSRoundUp32B(srcSize);
        if (file.read(srcs[i], readSize, offset) != static_cast<s32>(readSize)) {
            updateExchange.right(-1);
            return;
        }

        if (!updateExchange.right(srcSize)) {
            return;
        }
    }

    updateExchange.right(0);
}

static void *handle(void *UNUSED(arg)) {
    while (true) {
        const char *path = startExchange.right({});
        read(path);
    }
}

static void init() {
    OSCreateThread(&thread, handle, nullptr, stack + sizeof(stack), sizeof(stack), 24, 0);
    OSResumeThread(&thread);
}

static bool load(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap) {
    startExchange.left(path);

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
    } else {
        decoder.reset(new (heap, 0x4) LZMADecoder(src, srcSize, heap));
    }

    src += decoder->headerSize();
    srcSize -= decoder->headerSize();

    while (decoder->ok() && decoder->decode(src, srcSize)) {
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

} // namespace SP::DVDDecompLoader

extern "C" void DVDDecompLoader_init(void) {
    SP::DVDDecompLoader::init();
}

extern "C" bool DVDDecompLoader_load(const char *path, u8 **dst, size_t *dstSize, EGG_Heap *heap) {
    return SP::DVDDecompLoader::load(path, dst, dstSize, reinterpret_cast<EGG::Heap *>(heap));
}
