#pragma once

#include <Common.hh>

#include <expected>

namespace SP::Net {

class Socket {
public:
    virtual std::expected<std::optional<u16>, const wchar_t *> read(u8 *message, u16 maxSize) = 0;
    virtual std::expected<void, const wchar_t *> write(const u8 *message, u16 size) = 0;
};

} // namespace SP::Net
