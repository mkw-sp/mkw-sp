#include "ShaUtil.hh"

#include <cstdio>

std::array<char, 0x28 + 1> sha1ToHex(const Sha1 &sha1) {
    std::array<char, 0x28 + 1> hex;

    const u8 *data = sha1.data();
    for (u32 i = 0; i < sha1.size(); ++i) {
        sprintf(hex.data() + (i * 2), "%02x", (*(data + i)) & 0xff);
    }

    return hex;
}
