#pragma once

#include <Common.hh>

#include <optional>
#include <span>

namespace SP {

alignas(4) constexpr u8 U8_MAGIC[4] = {0x55, 0xAA, 0x38, 0x2D};

struct U8Header {
    u32 magic;
    u32 nodeOffset;
    u32 metaSize;
    u32 dataOffset;
};

struct U8Node {
    bool isDir;
    u32 nameOffset;
    u32 dataOffset;
    u32 size;
};

class U8Cursor {
public:
    U8Cursor(std::span<const u8> buffer) : m_buffer(buffer){};

    size_t position() const;
    void setPosition(size_t newPos);

    std::optional<u8> readByte();
    std::optional<bool> readBool();
    std::optional<u32> readU24();
    std::optional<u32> readU32();

    const char *readString(size_t tableStart, size_t tableOffset);

    std::optional<U8Header> readU8Header();
    std::optional<U8Node> readNode();

private:
    template <size_t N>
    std::optional<std::array<u8, N>> read();

    std::span<const u8> m_buffer;
    size_t m_currentPos = 0;
};

} // namespace SP
