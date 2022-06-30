#pragma once

#include "sp/Decoder.hh"

#include <egg/core/eggHeap.hh>

#include <optional>

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
    static std::optional<u32> GetDecodedSize(const u8 *src, size_t srcSize);
    static std::optional<u32> Decode(const u8 *src, size_t srcSize, u8 *dst, size_t dstSize);

    static const size_t HEADER_SIZE = 4 * sizeof(u32);

private:
    YAZDecoder(u8 *dst, size_t dstSize);

    bool process(const u8 *src, size_t &srcOffset);

    bool m_owning = true;
    u8 *m_dst = nullptr;
    size_t m_dstSize = 0;
    size_t m_dstOffset = 0;
    State m_state = State::GroupHeader;
    u8 m_groupHeaderIndex = 7;
    u8 m_groupHeader;
    u16 m_refSize;
    u16 m_refOffset;
    bool m_ok = true;

    static const u32 YAZ0_MAGIC;
    static const u32 YAZ1_MAGIC;
};

} // namespace SP
