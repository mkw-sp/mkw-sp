#include "WBZDecoder.hh"

#include "sp/U8Iterator.hh"
#include "sp/WU8Library.hh"

#include <common/Bytes.hh>
#include <egg/core/eggSystem.hh>
extern "C" {
#include <revolution.h>
}

#include <cstring>

// WBZaWU8a
constexpr u64 WBZ_MAGIC = 6287687403886164065;

namespace SP {

void *WBZDecoder::Alloc(void *heap_erased, int item, int count) {
    auto *heap = reinterpret_cast<EGG::Heap *>(heap_erased);
    return heap->alloc(item * count, -0x4);
}

void WBZDecoder::Free(void *heap_erased, void *addr) {
    auto *heap = reinterpret_cast<EGG::Heap *>(heap_erased);
    return heap->free(addr);
}

WBZDecoder::WBZDecoder(const u8 * /* src */, size_t /* srcSize */, EGG::Heap *heap)
    : m_wu8Buf(HeapAllocator<u8>({heap})) {
    memset(&m_stream, 0, sizeof(m_stream));
    m_stream.bzalloc = WBZDecoder::Alloc;
    m_stream.bzfree = WBZDecoder::Free;
    m_stream.opaque = heap;

    m_heap = heap;
    m_done = false;
    m_ok = BZ2_bzDecompressInit(&m_stream, /* Verbosity */ 0, /* Small */ false) == BZ_OK;
}

WBZDecoder::~WBZDecoder() = default;

bool WBZDecoder::decode(const u8 *src, size_t size) {
    assert(m_ok && !m_done);

    std::array<char, 0x5000> decodeBuf;

    m_stream.next_in = reinterpret_cast<char *>(const_cast<u8 *>(src));
    m_stream.avail_in = size;

    do {
        m_stream.next_out = decodeBuf.data();
        m_stream.avail_out = decodeBuf.size();
    } while (processOutBuffer(BZ2_bzDecompress(&m_stream), decodeBuf));

    if (!m_ok) {
        BZ2_bzDecompressEnd(&m_stream);
        return false;
    } else if (m_done) {
        BZ2_bzDecompressEnd(&m_stream);
        return DecodeWU8(m_wu8Buf);
    } else {
        return true;
    }
}

void WBZDecoder::release(u8 **dst, size_t *dstSize) {
    assert(m_done && m_ok);

    // We have to leak the vector's internal memory, but std::vector
    // does not supply an interface to avoid deallocating it's memory.
    u8 *leakedBuf = new (m_heap, 0x20) u8[m_wu8Buf.size()];
    memcpy(leakedBuf, m_wu8Buf.data(), m_wu8Buf.size());

    *dst = leakedBuf;
    *dstSize = m_wu8Buf.size();
}

size_t WBZDecoder::headerSize() const {
    return 0x10;
}

bool WBZDecoder::ok() const {
    return m_ok;
}

bool WBZDecoder::done() const {
    return m_done;
}

bool WBZDecoder::CheckMagic(u64 magic) {
    return magic == WBZ_MAGIC;
}

bool WBZDecoder::processOutBuffer(int retCode, std::span<char> decodeBuf) {
    auto writtenSize = std::end(decodeBuf) - m_stream.avail_out;
    m_wu8Buf.insert(m_wu8Buf.end(), std::begin(decodeBuf), writtenSize);

    if (retCode == BZ_OK) {
        if (m_stream.avail_out > 0) {
            return false;
        } else {
            return true;
        }
    } else if (retCode == BZ_STREAM_END) {
        // Set m_done to signal that we need to move to WU8 decoding.
        m_done = true;
        return false;
    } else {
        m_ok = false;
        return false;
    }
}

} // namespace SP
