#pragma once

#include "Decoder.hh"

#include "sp/HeapAllocator.hh"

#include <span>
#include <vector>
#include <vendor/bzip2/bzlib.h>

namespace SP {

class WBZDecoder : public Decoder {
public:
    WBZDecoder(const u8 *src, size_t srcSize, EGG::Heap *heap);
    ~WBZDecoder() override;
    bool decode(const u8 *src, size_t size) override;
    void release(u8 **dst, size_t *dstSize) override;
    bool ok() const override;
    bool done() const override;
    size_t headerSize() const override;

    static bool CheckMagic(u64 magic);

private:
    bool processOutBuffer(int retCode, std::span<char> decodeBuf);
    bool decodeWU8();

    static void *Alloc(void *heap_erased, int item, int count);
    static void Free(void *heap, void *addr);

    EGG::Heap *m_heap;
    bz_stream m_stream;
    std::vector<u8, HeapAllocator<u8>> m_wu8Buf;

    bool m_ok;
    bool m_done;
};

} // namespace SP
