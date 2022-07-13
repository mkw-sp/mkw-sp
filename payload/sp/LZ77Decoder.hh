#pragma once

#include "sp/Decoder.hh"

#include <egg/core/eggHeap.hh>

namespace SP {

class LZ77Decoder : public Decoder {
private:
    enum class State {
        GroupHeader,
        Copy,
        RefHeader0,
        RefHeader1,
        RefCopy,
    };

public:
    LZ77Decoder(const u8 *src, size_t srcSize, EGG::Heap *heap);
    ~LZ77Decoder() override;
    bool decode(const u8 *src, size_t size) override;
    void release(u8 **dst, size_t *dstSize) override;
    bool ok() const override;
    bool done() const override;
    size_t headerSize() const override;

    static bool CheckMagic(u32 magic);

private:
    bool process(const u8 *src, size_t &srcOffset);

    size_t m_headerSize = 0;
    u8 *m_dst = nullptr;
    size_t m_dstSize = 0;
    size_t m_dstOffset = 0;
    State m_state = State::GroupHeader;
    u8 m_groupHeaderIndex = 7;
    u8 m_groupHeader;
    u16 m_refSize;
    u16 m_refOffset;
    bool m_ok = true;
};

} // namespace SP
