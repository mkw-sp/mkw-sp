#include "U8Cursor.hh"

#include <cstring>

namespace SP {

template <size_t N>
std::optional<std::array<u8, N>> U8Cursor::read() {
    std::array<u8, N> buf;

    auto start = m_buffer.begin() + m_currentPos;
    if ((start + N) >= m_buffer.end()) {
        return std::nullopt;
    }

    std::copy(start, start + N, buf.begin());
    m_currentPos += N;
    return buf;
}

size_t U8Cursor::position() const {
    return m_currentPos;
}

void U8Cursor::setPosition(size_t newPos) {
    m_currentPos = newPos;
}

std::optional<u8> U8Cursor::readByte() {
    std::optional<std::array<u8, 1>> byte_a;
    if ((byte_a = read<1>()).has_value()) {
        return (*byte_a)[0];
    }

    return std::nullopt;
}

std::optional<bool> U8Cursor::readBool() {
    auto byte = readByte();
    if (!byte.has_value()) {
        return std::nullopt;
    } else if (*byte == 0) {
        return false;
    } else if (*byte == 1) {
        return true;
    } else {
        return std::nullopt;
    }
}

std::optional<u32> U8Cursor::readU24() {
    auto bytes = read<3>();
    if (!bytes.has_value()) {
        return std::nullopt;
    }

    u32 out = 0;
    out |= (((*bytes)[0] & 0xFF) << 16);
    out |= (((*bytes)[1] & 0xFF) << 8);
    out |= (((*bytes)[2] & 0xFF) << 0);
    return out;
}

std::optional<u32> U8Cursor::readU32() {
    auto bytes = read<4>();
    if (!bytes.has_value()) {
        return std::nullopt;
    }

    u32 out = 0;
    memcpy(&out, bytes->data(), 4);
    return out;
}

const char *U8Cursor::readString(size_t tableStart, size_t tableOffset) {
    auto *buffer = reinterpret_cast<const char *>(m_buffer.data());
    return &buffer[tableStart + tableOffset];
}

std::optional<U8Header> U8Cursor::readU8Header() {
    U8Header header;
    auto magic = readU32();
    auto nodeOffset = readU32();
    auto metaSize = readU32();
    auto dataOffset = readU32();

    if (!magic || !nodeOffset || !metaSize || !dataOffset) {
        return std::nullopt;
    }

    header.magic = *magic;
    header.nodeOffset = *nodeOffset;
    header.metaSize = *metaSize;
    header.dataOffset = *dataOffset;
    return header;
}

std::optional<U8Node> U8Cursor::readNode() {
    U8Node node;

    auto isDir = readByte();
    auto nameOffset = readU24();
    auto dataOffset = readU32();
    auto size = readU32();
    if (!isDir || !nameOffset || !dataOffset || !size) {
        return std::nullopt;
    }

    node.isDir = *isDir;
    node.nameOffset = *nameOffset;
    node.dataOffset = *dataOffset;
    node.size = *size;
    return node;
}

} // namespace SP
