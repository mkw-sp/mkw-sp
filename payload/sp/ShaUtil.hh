#pragma once

#include <vendor/libhydrogen/hydrogen.h>

#include <expected>
#include <string_view>

#include <Common.hh>

using Sha1 = std::array<u8, 0x14>;
constexpr std::array<char, 40 + 1> sha1ToHex(const Sha1 &sha1) {
    std::array<char, 40 + 1> out;
    hydro_bin2hex(out.data(), out.size(), sha1.data(), sha1.size());
    return out;
}

constexpr std::expected<Sha1, const char *> sha1FromHex(std::string_view sv) {
    Sha1 out;
    u32 val = hydro_hex2bin(out.data(), out.size(), sv.data(), sv.size(), nullptr, nullptr);
    if (val == out.size()) {
        return out;
    } else {
        return std::unexpected("Failed to parse sha1 from hex!");
    }
};
