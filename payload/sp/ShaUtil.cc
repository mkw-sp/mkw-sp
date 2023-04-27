#include "ShaUtil.hh"

#include <charconv>

namespace SP {

FixedString<0x28 + 1> SHA1ToHex(std::span<const u8, 0x14> bytes) {
    FixedString<0x28 + 1> hex;

    for (size_t i = 0; i < bytes.size(); ++i) {
        std::span s{&hex.m_buf[i * 2], 2};
        std::to_chars(s.data(), s.data() + s.size(), bytes[i], 16);
    }

    return hex;
}

std::expected<std::array<u8, 0x14>, const char *> SHA1FromHex(std::string_view view) {
    if (view.size() != (0x14 * 2)) {
        return std::unexpected("Invalid SHA1 length!");
    }

    std::array<u8, 0x14> sha;
    for (size_t i = 0; i < view.size(); i += 2) {
        auto s = view.substr(i, 2);
        std::from_chars(s.data(), s.data() + s.size(), sha[i / 2], 16);
    }
    return sha;
}

} // namespace SP
