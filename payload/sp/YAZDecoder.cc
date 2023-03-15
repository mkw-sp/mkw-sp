#include "YAZDecoder.hh"

#include <common/Bytes.hh>

#include <algorithm>

namespace SP {

const u32 YAZDecoder::YAZ0_MAGIC = 0x59617a30;
const u32 YAZDecoder::YAZ1_MAGIC = 0x59617a31;

YAZDecoder::YAZDecoder(const u8 *src, size_t srcSize, EGG::Heap *heap) {
    if (srcSize < HEADER_SIZE) {
        m_ok = false;
        return;
    }
    m_dstSize = Bytes::Read<u32>(src, 0x4);
    m_dst = new (heap, 0x20) u8[m_dstSize];
}

YAZDecoder::YAZDecoder(u8 *dst, size_t dstSize) : m_owning(false), m_dst(dst), m_dstSize(dstSize) {}

YAZDecoder::~YAZDecoder() {
    if (m_owning) {
        delete[] m_dst;
    }
    m_dst = nullptr;
    m_dstSize = 0;
}

bool YAZDecoder::process(const u8 *src, size_t &srcOffset) {
    u8 val;
    switch (m_state) {
    case State::GroupHeader:
        m_groupHeaderIndex++;
        if (m_groupHeaderIndex == 8) {
            m_groupHeader = src[srcOffset++];
            m_groupHeaderIndex = 0;
        }
        if (m_groupHeader >> (7 - m_groupHeaderIndex) & 1) {
            m_state = State::Copy;
        } else {
            m_state = State::RefHeader0;
        }
        return true;
    case State::Copy:
        m_dst[m_dstOffset++] = src[srcOffset++];
        m_state = State::GroupHeader;
        return true;
    case State::RefHeader0:
        val = src[srcOffset++];
        m_refOffset = val << 8 & 0xf00;
        m_refSize = (val >> 4) + 0x2;
        m_state = State::RefHeader1;
        return true;
    case State::RefHeader1:
        val = src[srcOffset++];
        m_refOffset += val + 0x1;
        if (m_refOffset > m_dstOffset) {
            return false;
        }
        if (m_refSize == 0x2) {
            m_state = State::RefHeader2;
        } else {
            m_state = State::RefCopy;
        }
        return true;
    case State::RefHeader2:
        val = src[srcOffset++];
        m_refSize = val + 0x12;
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

bool YAZDecoder::decode(const u8 *src, size_t srcSize) {
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

        if (srcOffset == srcSize &&
                ((m_state != State::GroupHeader || m_groupHeaderIndex == 7) &&
                        m_state != State::RefCopy)) {
            return true;
        }
    } while (process(src, srcOffset));

    m_ok = false;
    return false;
}

void YAZDecoder::release(u8 **dst, size_t *dstSize) {
    assert(ok() && done() && m_dst != nullptr);

    *dst = m_dst;
    *dstSize = m_dstSize;
    m_dst = nullptr;
    m_dstSize = 0;
}

bool YAZDecoder::ok() const {
    return m_ok;
}

bool YAZDecoder::done() const {
    return m_dstOffset == m_dstSize;
}

size_t YAZDecoder::headerSize() const {
    return HEADER_SIZE;
}

bool YAZDecoder::CheckMagic(u32 magic) {
    return magic == YAZ0_MAGIC || magic == YAZ1_MAGIC;
}

std::optional<u32> YAZDecoder::GetDecodedSize(const u8 *src, size_t srcSize) {
    if (srcSize < HEADER_SIZE) {
        return {};
    }
    u32 magic = Bytes::Read<u32>(src, 0x0);
    if (!CheckMagic(magic)) {
        return {};
    }
    return Bytes::Read<u32>(src, 0x4);
}

std::optional<u32> YAZDecoder::Decode(const u8 *src, size_t srcSize, u8 *dst, size_t dstSize) {
    auto tmp = GetDecodedSize(src, srcSize);
    if (!tmp) {
        return {};
    }
    dstSize = std::min(static_cast<u32>(dstSize), *tmp);
    YAZDecoder decoder(dst, dstSize);
    if (!decoder.decode(src + HEADER_SIZE, srcSize - HEADER_SIZE)) {
        return {};
    }
    if (!decoder.done()) {
        return {};
    }
    return dstSize;
}

} // namespace SP
