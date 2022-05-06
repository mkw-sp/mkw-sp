#include "LZMADecoder.hh"

#include "sp/Bytes.hh"

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

LZMADecoder::LZMADecoder(const u8 *src, size_t srcSize, EGG::Heap *heap) : m_allocator(heap),
        m_done(false) {
    LzmaDec_Construct(&m_dec);
    if (srcSize < HEADER_SIZE || Bytes::Read<u64, std::endian::little>(src, LZMA_PROPS_SIZE) >
            SIZE_MAX) {
        m_ok = false;
        return;
    }
    m_ok = LzmaDec_AllocateProbs(&m_dec, src, LZMA_PROPS_SIZE, &m_allocator) == SZ_OK;
    LzmaDec_Init(&m_dec);
    m_dec.dicBufSize = Bytes::Read<u64, std::endian::little>(src, LZMA_PROPS_SIZE);
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

    size_t srcOffset = srcSize;
    ELzmaStatus status;
    if (LzmaDec_DecodeToDic(&m_dec, m_dec.dicBufSize, src, &srcOffset, LZMA_FINISH_ANY, &status) !=
            SZ_OK) {
        m_ok = false;
        return false;
    }
    if (m_dec.dicPos == m_dec.dicBufSize) {
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
