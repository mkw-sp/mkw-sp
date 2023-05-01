#include "ShaUtil.hh"

#include <vendor/libhydrogen/hydrogen.h>

std::array<char, 0x28 + 1> sha1ToHex(const Sha1 &sha1) {
    std::array<char, 40 + 1> out;
    hydro_bin2hex(out.data(), out.size(), sha1.data(), sha1.size());
    return out;
};

std::expected<Sha1, const char *> sha1FromHex(std::string_view sv) {
    Sha1 out;
    u32 val = hydro_hex2bin(out.data(), out.size(), sv.data(), sv.size(), nullptr, nullptr);
    if (val == out.size()) {
        return out;
    } else {
        return std::unexpected("Failed to parse sha1 from hex!");
    }
}
