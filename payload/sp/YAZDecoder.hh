#pragma once

#include "sp/Decoder.hh"

#include <egg/core/eggHeap.hh>

namespace SP {

class YAZDecoder : public Decoder {
private:
    enum class State {
        GroupHeader,
        Copy,
        RefHeader0,
        RefHeader1,
        RefHeader2,
        RefCopy,
    };

public:
    YAZDecoder(const u8 *src, size_t srcSize, EGG::Heap *heap);
    ~YAZDecoder() override;
    bool decode(const u8 *src, size_t size) override;
    void release(u8 **dst, size_t *dstSize) override;
    bool ok() const override;
    bool done() const override;
    size_t headerSize() const override;

    static bool CheckMagic(u32 magic);

    static const size_t HEADER_SIZE = 4 * sizeof(u32);

private:
    bool process(const u8 *src, size_t &srcOffset);

    u8 *m_dst;
    size_t m_dstSize;
    size_t m_dstOffset;
    State m_state;
    u8 m_groupHeaderIndex;
    u8 m_groupHeader;
    u16 m_refSize;
    u16 m_refOffset;
    bool m_ok;

    static const u32 YAZ0_MAGIC;
    static const u32 YAZ1_MAGIC;
};

} // namespace SP
