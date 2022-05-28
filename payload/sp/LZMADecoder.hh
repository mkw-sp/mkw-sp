#pragma once

#include "sp/Decoder.hh"

#include <egg/core/eggHeap.hh>

extern "C" {
#include <lzma/LzmaDec.h>
}

namespace SP {

class LZMADecoder : public Decoder {
private:
    class LZMAAllocator : public ISzAlloc {
    public:
        LZMAAllocator(EGG::Heap *heap);
        ~LZMAAllocator();

    private:
        static void *Alloc(const ISzAlloc *base, size_t size);
        static void Free(const ISzAlloc *base, void *address);

        EGG::Heap *m_heap;
    };

public:
    LZMADecoder(const u8 *src, size_t srcSize, EGG::Heap *heap);
    ~LZMADecoder() override;
    bool decode(const u8 *src, size_t size) override;
    void release(u8 **dst, size_t *dstSize) override;
    bool ok() const override;
    bool done() const override;
    size_t headerSize() const override;

    static const size_t HEADER_SIZE = LZMA_PROPS_SIZE + sizeof(u64);

private:
    EGG::Heap *m_heap;
    LZMAAllocator m_allocator;
    CLzmaDec m_dec;
    bool m_knownDstSize = true;
    bool m_ok = false;
    bool m_done = false;
};

} // namespace SP
