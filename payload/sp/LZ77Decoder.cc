#include "LZ77Decoder.hh"

#include <common/Bytes.hh>

namespace SP {

LZ77Decoder::LZ77Decoder(const u8 *src, size_t srcSize, EGG::Heap *heap) {
    if (srcSize < sizeof(u32)) {
        m_ok = false;
        return;
    }
    m_dstSize = Bytes::Read<u32, std::endian::little>(src, 0x0) >> 8;
    if (m_dstSize == 0) {
        if (srcSize < 2 * sizeof(u32)) {
            m_ok = false;
            return;
        }
        m_headerSize = 2 * sizeof(u32);
        m_dstSize = Bytes::Read<u32, std::endian::little>(src, 0x4);
    } else {
        m_headerSize = sizeof(u32);
    }
    m_dst = new (heap, 0x20) u8[m_dstSize];
}

LZ77Decoder::~LZ77Decoder() {
    delete[] m_dst;
    m_dst = nullptr;
    m_dstSize = 0;
}

bool LZ77Decoder::process(const u8 *src, size_t &srcOffset) {
    u8 val;
    switch (m_state) {
    case State::GroupHeader:
        m_groupHeaderIndex++;
        if (m_groupHeaderIndex == 8) {
            m_groupHeader = src[srcOffset++];
            m_groupHeaderIndex = 0;
        }
        if (m_groupHeader >> (7 - m_groupHeaderIndex) & 1) {
            m_state = State::RefHeader0;
        } else {
            m_state = State::Copy;
        }
        return true;
    case State::Copy:
        m_dst[m_dstOffset++] = src[srcOffset++];
        m_state = State::GroupHeader;
        return true;
    case State::RefHeader0:
        val = src[srcOffset++];
        m_refOffset = val << 8 & 0xf00;
        m_refSize = (val >> 4) + 0x3;
        m_state = State::RefHeader1;
        return true;
    case State::RefHeader1:
        val = src[srcOffset++];
        m_refOffset += val + 0x1;
        if (m_refOffset > m_dstOffset) {
            return false;
        }
        m_state = State::RefCopy;
        return true;
    case State::RefCopy:
        val = m_dst[m_dstOffset - m_refOffset];
        m_dst[m_dstOffset++] = val;
        m_refSize--;
        if (m_refSize == 0x0) {
            m_state = State::GroupHeader;
        }
        return true;
    }

    assert(false);
}

bool LZ77Decoder::decode(const u8 *src, size_t srcSize) {
    assert(ok() && !done());

    size_t srcOffset = 0;
    do {
        if (m_dstOffset == m_dstSize) {
            if (m_state == State::GroupHeader) {
                return true;
            }
            m_ok = false;
            return false;
        }

        if (srcOffset == srcSize && ((m_state != State::GroupHeader || m_groupHeaderIndex == 7) &&
                m_state != State::RefCopy)) {
            return true;
        }
    } while (process(src, srcOffset));

    m_ok = false;
    return false;
}

void LZ77Decoder::release(u8 **dst, size_t *dstSize) {
    assert(ok() && done() && m_dst != nullptr);

    *dst = m_dst;
    *dstSize = m_dstSize;
    m_dst = nullptr;
    m_dstSize = 0;
}

bool LZ77Decoder::ok() const {
    return m_ok;
}

bool LZ77Decoder::done() const {
    return m_dstOffset == m_dstSize;
}

size_t LZ77Decoder::headerSize() const {
    return m_headerSize;
}

bool LZ77Decoder::CheckMagic(u32 magic) {
    return (magic & 0xff) == 0x10;
}

} // namespace SP
