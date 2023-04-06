#include "ShaUtil.hh"

#include <cstdio>
#include <cstdlib>

std::array<char, 0x28 + 1> sha1ToHex(const Sha1 &sha1) {
    std::array<char, 0x28 + 1> hex;

    const u8 *data = sha1.data();
    for (u32 i = 0; i < sha1.size(); ++i) {
        snprintf(hex.data() + (i * 2), 3, "%02x", (*(data + i)) & 0xff);
    }

    return hex;
}

std::expected<Sha1, const char *> sha1FromSv(std::string_view sv) {
    if (sv.size() != (0x14 * 2)) {
        return std::unexpected("Invalid SHA1 length!");
    }

    Sha1 sha;
    char tByte[3];

    for (u8 i = 0; i < sv.size(); i += 2) {
        tByte[0] = sv[i];
        tByte[1] = sv[i + 1];
        tByte[2] = '\0';

        sha[i / 2] = strtol(tByte, nullptr, 16);
    }

    return sha;
}
