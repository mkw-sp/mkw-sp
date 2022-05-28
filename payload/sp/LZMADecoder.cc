#include "LZMADecoder.hh"

#include <common/Bytes.hh>

#include <cstring>

namespace SP {

LZMADecoder::LZMAAllocator::LZMAAllocator(EGG::Heap *heap) : m_heap(heap) {
    ISzAlloc::Alloc = Alloc;
    ISzAlloc::Free = Free;    
}

LZMADecoder::LZMAAllocator::~LZMAAllocator() {}

void *LZMADecoder::LZMAAllocator::Alloc(const ISzAlloc *base, size_t size) {
    const LZMAAllocator *allocator = reinterpret_cast<const LZMAAllocator *>(base);
    return allocator->m_heap->alloc(size, 0x4);
}

void LZMADecoder::LZMAAllocator::Free(const ISzAlloc *base, void *address) {
    const LZMAAllocator *allocator = reinterpret_cast<const LZMAAllocator *>(base);
    allocator->m_heap->free(address);
}

LZMADecoder::LZMADecoder(const u8 *src, size_t srcSize, EGG::Heap *heap) : m_heap(heap),
        m_allocator(heap) {
    LzmaDec_Construct(&m_dec);
    if (srcSize < HEADER_SIZE) {
        return;
    }
    u64 dstSize = Bytes::Read<u64, std::endian::little>(src, LZMA_PROPS_SIZE);
    if (dstSize == UINT64_MAX) {
        m_knownDstSize = false;
    } else if (dstSize > SIZE_MAX) {
        return;
    }
    m_ok = LzmaDec_AllocateProbs(&m_dec, src, LZMA_PROPS_SIZE, &m_allocator) == SZ_OK;
    LzmaDec_Init(&m_dec);
    m_dec.dicBufSize = m_knownDstSize ? dstSize : 0x20000 /* 128 KiB */;
    m_dec.dic = new (heap, 0x20) u8[m_dec.dicBufSize];
}

LZMADecoder::~LZMADecoder() {
    delete[] m_dec.dic;
    m_dec.dic = nullptr;
    m_dec.dicBufSize = 0;
    LzmaDec_FreeProbs(&m_dec, &m_allocator);
}

bool LZMADecoder::decode(const u8 *src, size_t srcSize) {
    assert(ok() && !done());

    ELzmaStatus status;
    do {
        if (!m_knownDstSize && m_dec.dicPos == m_dec.dicBufSize) {
            size_t dicBufSize = m_dec.dicBufSize;
            m_dec.dicBufSize += 0x20000;
            if (m_heap->resizeForMBlock(m_dec.dic, m_dec.dicBufSize) != m_dec.dicBufSize) {
                u8 *dic = m_dec.dic;
                m_dec.dic = new (m_heap, 0x20) u8[m_dec.dicBufSize];
                memcpy(m_dec.dic, dic, dicBufSize);
                delete[] dic;
            }
        }
        size_t srcOffset = srcSize;
        ELzmaFinishMode finishMode = m_knownDstSize ? LZMA_FINISH_END : LZMA_FINISH_ANY;
        if (LzmaDec_DecodeToDic(&m_dec, m_dec.dicBufSize, src, &srcOffset, finishMode, &status) !=
                SZ_OK) {
            m_ok = false;
            return false;
        }
        src += srcOffset;
        srcSize -= srcOffset;
        if (srcSize != 0 && (m_knownDstSize || status == LZMA_STATUS_FINISHED_WITH_MARK)) {
            m_ok = false;
            return false;
        }
    } while (srcSize != 0);
    if (!m_knownDstSize && status == LZMA_STATUS_FINISHED_WITH_MARK) {
        m_done = true;
    } else if (m_knownDstSize && m_dec.dicPos == m_dec.dicBufSize) {
        if (status != LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK) {
            m_ok = false;
            return false;
        }
        m_done = true;
    }
    return true;
}

void LZMADecoder::release(u8 **dst, size_t *dstSize) {
    assert(ok() && done() && m_dec.dic != nullptr && m_dec.dicBufSize != 0);

    if (!m_knownDstSize) {
        if (m_heap->resizeForMBlock(m_dec.dic, m_dec.dicPos) == m_dec.dicPos) {
            m_dec.dicBufSize = m_dec.dicPos;
        }
    }
    *dst = m_dec.dic;
    *dstSize = m_dec.dicBufSize;
    m_dec.dic = nullptr;
    m_dec.dicBufSize = 0;
}

bool LZMADecoder::ok() const {
    return m_ok;
}

bool LZMADecoder::done() const {
    return m_done;
}

size_t LZMADecoder::headerSize() const {
    return HEADER_SIZE;
}

} // namespace SP
